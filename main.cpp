/* Includes */
#include <gdkmm.h>
#include <cairomm.h>
#include <giomm.h>
#include <gtkmm.h>
#include "LinAlg.cpp"

/* Defines */
#define PI 3.1415962

/* Globals */
Cairo::RefPtr<Cairo::Context> cairoContext;
int frameNumber = 0;

/* Interpolation Functions. */
double getAngle(int frameNumber){
    /* Gets the rotation angle from a given frame number.*/
    return PI*frameNumber/(double)12;
};

double getScale(int frameNumber){
    /* Get the scale factor for a given frame number. */
    // Get how many cycles have occured. Each cycle is 
    // 24 frames. 
    // I am using piecewise linear functions to change scale.

    // (0 Frames, 1) -> (24*4 Frames, 4) -> (24*8 Frames, 1/4) -> (24*12 Frames, 1), repeat. 
    
    
    double scale;
    int clampedFrame = frameNumber %(24*12); 
    if(0 <= clampedFrame && clampedFrame < 96){
        scale = (3.0/96.0)*clampedFrame +1.0;
    } else if (96 <= clampedFrame && clampedFrame < 192){
        scale = (-3.75)/(96.0)*(clampedFrame-96) + 4.0;
    } else if (192 <= clampedFrame && clampedFrame < 288){
        scale = (0.75)/(96.0)*(clampedFrame-192)+0.25;
    }
    return scale;
};



int getFlatIndex(int xVal, int yVal, int rowWidth){
    return xVal+yVal*rowWidth;
}

void getPixPositionArray(int hRes, int vRes, Vec<double,3>* pixPositionArray){
    for(int pix =0; pix < hRes*vRes; pix++){
        // Keep in mind the column is the X value. 
        int rowNum = pix/hRes;
        int colNum = pix - hRes*rowNum;
        double initVals[3] = { (double)colNum/hRes, 1.0-(double(rowNum)/vRes),1.0};
        pixPositionArray[pix] = Vec<double,3>(initVals);
    }
    // We now have an array representing the positions for each pixel! 
}

void getTransform(double theta, double scale, Transform<double,3,3>* T){
    // Translate the image down and to the left by (-1/2, -1/2) to center it. 
    // Transform the image by rotation Theta (inverse)
    // Scale the image by 'scale' (inverse)
    // Translate the image back up and to the right by 1/2, 1/2. 

    // This transformation is given by the matrix 
    //
    //       cos(theta)/scale , sin(theta)/scale , - (sin(theta) + cos(theta) - scale)/(2*scale)
    // T =  -sin(theta)/scale, cos(theta)/scale , - (cos(theta) - sin(theta) - scale)/(2*scale)
    //         0,                              0,                                   1
    //

    double initVals[3][3] = {cos(theta)/scale, sin(theta)/scale, -(sin(theta)+cos(theta)-scale)/(2*scale),
    -sin(theta)/scale, cos(theta)/scale, -(cos(theta)-sin(theta)-scale)/(2*scale),
    0, 0, 1};
    *T = Transform<double,3,3>(initVals);
}

void fillColorBuffer(int imageHRes, int imageVRes, int targetHRes, int targetVRes, Vec<double,3>* pixFinalPositions, Glib::RefPtr<Gdk::Pixbuf> drawBuffer, Glib::RefPtr<Gdk::Pixbuf> originalBuffer){
    int n_channels = originalBuffer->get_n_channels();
    int rowStride = originalBuffer->get_rowstride();
    guint8* colorBuff = originalBuffer->get_pixels();

    guint8* outBuff = drawBuffer->get_pixels();
    for(int pix = 0; pix < targetHRes*targetVRes; pix++){
        int pixY = pix/targetHRes;
        int pixX = pix - targetHRes*pixY;
        guint8* outP = outBuff + pixY*rowStride + pixX*n_channels;
        // if outX > 1 or outX < 0, out of bounds. if outY > 1 or outY < 0, out of bounds. 
        double outX = pixFinalPositions[pix].components[0];
        double outY = pixFinalPositions[pix].components[1];
        if(outX > 1 || outX < 0){
            outP[0] = (guint8)0;
            outP[1] = (guint8)0;
            outP[2] = (guint8)0;
        } else if (outY>1 || outY<0){
            outP[0] = (guint8)0;
            outP[1] = (guint8)0;
            outP[2] = (guint8)0;
        } else {
            int sourceX = floor(outX*imageHRes);
            int sourceY = floor((1-outY)*imageVRes);

            guint8* sourcePixel = colorBuff + sourceY*rowStride + sourceX*n_channels;

            outP[0] = sourcePixel[0];
            outP[1] = sourcePixel[1];
            outP[2] = sourcePixel[2];
        }
        // Transform the pixels final position to its position in an index. 
    }
}

void transformVecArray(Transform<double,3,3>* T, Vec<double,3>* inVecs, Vec<double,3>* outVecs, int numVecs){
    for(int vec = 0; vec < numVecs; vec++){
        // Applies transformation T to all elements of inVecs and stores result in outVecs. 
        Vec<double,3> v = inVecs[vec]; 
        outVecs[vec] = (*T)*inVecs[vec];
    }
}


class ppmDraw : public Gtk::DrawingArea{
    /* A class containing all the info needed to run the basic draw loop */
    public:
        ppmDraw();
    protected:
        virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>&cairoContext);
        Glib::RefPtr<Gdk::Pixbuf> originalBuffer;
        Glib::RefPtr<Gdk::Pixbuf> drawBuffer;
        Transform<double,3,3> T;
        int windowH;
        int windowV;
        int imageH;
        int imageV;
        Vec<double,3>* pixelPositions;
        Vec<double,3>* transformedPositions;
};

class MainWindow : public Gtk::Window{
    /* A class containing the basic info needed to make a Gtk Window.*/
    public:
        MainWindow(int hRes, int vRes);
    protected:
        ppmDraw ppmCanvas;
        int hRes;
        int vRes;
};

ppmDraw::ppmDraw(){
    /* Initialization of values. */
    originalBuffer = Gdk::Pixbuf::create_from_file("./thisIsFine.ppm");
    drawBuffer = originalBuffer->copy();

    windowH = drawBuffer->get_width();
    windowV = drawBuffer->get_height();
    imageH = originalBuffer->get_width();
    imageV = originalBuffer->get_height();

    /* Fill the position array. */
    pixelPositions = (Vec<double,3>*)malloc(sizeof(Vec<double,3>)*windowH*windowV);
    getPixPositionArray(windowH,windowV,pixelPositions);

    /* Allocate memory for the transformed pixels. */
    transformedPositions = (Vec<double,3>*)malloc(sizeof(Vec<double,3>)*windowH*windowV);
    getPixPositionArray(windowH,windowV,transformedPositions);
};

bool ppmDraw::on_draw(const Cairo::RefPtr<Cairo::Context> &cairoContext){
    Gtk::Allocation allocation = get_allocation();
    Gdk::Cairo::set_source_pixbuf(cairoContext,drawBuffer,0);
    cairoContext->paint();

    // Get the transform we will use. 
    getTransform(getAngle(frameNumber),getScale(frameNumber),&T);

    // Apply the transform to all positions.
    transformVecArray(&T,pixelPositions,transformedPositions,windowH*windowV);

    // Get pixel colors
    fillColorBuffer(imageH,imageV,windowH,windowV,transformedPositions,drawBuffer,originalBuffer);

    /* Increment number of frames and queue a draw call.*/
    frameNumber++;

    queue_draw();
    return true;
};


MainWindow::MainWindow(int hRes, int vRes){
    // Initialize horizontal resolution of window, etc. 
    hRes = hRes;
    vRes = vRes;
   set_title("HW1");
   set_default_size(hRes,vRes);
   add(ppmCanvas);
   show_all();
}


int main(int argc, char* argv[]){
    // Run the program.
    Gtk::Main kit(argc, argv);
    // Hardcoded resolution because I am evil. 
    // You could pass it as an arg to main or get it 
    // from the PPM if you really cared. 
    MainWindow mainWindow = MainWindow(735,745);
    Gtk::Main::run(mainWindow);
    return 0;
}
