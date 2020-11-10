#include "MouseExample.h"
#include "examples.h"

#include <opencv2/opencv.hpp>
#include <opencv2/core/ocl.hpp>

// ----------------------------------------------------------------------

static void Help()
{
    printf("\nExamples of the Multitarget tracking algorithm\n"
           "Usage: \n"
           "          ./MultitargetTracker <path to movie file> [--example]=<number of example 0..7> [--start_frame]=<start a video from this position> [--end_frame]=<play a video to this position> [--end_delay]=<delay in milliseconds after video ending> [--out]=<name of result video file> [--show_logs]=<show logs> [--async]=<async pipeline> \n\n"
           "Press:\n"
           "\'m\' key for change mode: play|pause. When video is paused you can press any key for get next frame. \n\n"
           "Press Esc to exit from video \n\n"
           );
}

const char* keys =
{
    "{ @1             |../data/atrium.avi  | movie file | }"
    "{ e  example     |1                   | number of example 0 - MouseTracking, 1 - MotionDetector, 2 - FaceDetector, 3 - PedestrianDetector, 4 - OpenCV dnn objects detector, 5 - YOLO Darknet detector, 6 - YOLO TensorRT Detector | }"
    "{ sf start_frame |0                   | Start a video from this position | }"
    "{ ef end_frame   |0                   | Play a video to this position (if 0 then played to the end of file) | }"
    "{ ed end_delay   |0                   | Delay in milliseconds after video ending | }"
    "{ o  out         |                    | Name of result video file | }"
    "{ sl show_logs   |1                   | Show Trackers logs | }"
    "{ g gpu          |0                   | Use OpenCL acceleration | }"
    "{ a async        |1                   | Use 2 theads for processing pipeline | }"
};

// ----------------------------------------------------------------------

int main(int argc, char** argv)
{
    Help();

    cv::CommandLineParser parser(argc, argv, keys);

    bool useOCL = parser.get<int>("gpu") != 0;
    cv::ocl::setUseOpenCL(useOCL);
    std::cout << (cv::ocl::useOpenCL() ? "OpenCL is enabled" : "OpenCL not used") << std::endl;

    int exampleNum = parser.get<int>("example");
    int asyncPipeline = parser.get<int>("async");

	std::unique_ptr<VideoExample> detector;

    switch (exampleNum)
    {
    case 0:
        MouseTracking(parser);
        break;

    case 1:
        detector = std::make_unique<MotionDetectorExample>(parser);
        break;

    case 2:
		detector = std::make_unique<FaceDetectorExample>(parser);
        break;

    case 3:
		detector = std::make_unique<PedestrianDetectorExample>(parser);
        break;

	case 4:
		detector = std::make_unique<OpenCVDNNExample>(parser);
		break;

#ifdef BUILD_YOLO_LIB
	case 5:
		detector = std::make_unique<YoloDarknetExample>(parser);
		break;
#endif

#ifdef BUILD_YOLO_TENSORRT
	case 6:
		detector = std::make_unique<YoloTensorRTExample>(parser);
		break;
#endif

    default:
        std::cerr << "Wrong example number: " << exampleNum << std::endl;
        break;
    }

	if (detector.get())
		asyncPipeline ? detector->AsyncProcess() : detector->SyncProcess();

#ifndef SILENT_WORK
    cv::destroyAllWindows();
#endif
    return 0;
}
