/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_0.txt.                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Robert Eisele <robert@xarg.org>                              |
  +----------------------------------------------------------------------+
*/
//	http://www.santyago.pl/files/facedetect-1.0.1-opencv-2.2.0.patch
//	- face recog lib embedden http://libface.sourceforge.net/file/Examples.html
//	- http://www.cognotics.com/opencv/servo_2007_series/part_5/index.html
//	- opencv 2.2 kompatibel
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "php.h"
#include "php_facedetect.h"


//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/imgproc/imgproc.hpp>

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "opencv2/objdetect/objdetect.hpp"

using namespace std;
using namespace cv;


/*
String face_cascade_name = "/usr/local/share/OpenCV/haarcascades/haarcascade_frontalface_alt.xml";
String eyes_cascade_name = "/usr/local/share/OpenCV/haarcascades/haarcascade_eye.xml";
*/

CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;


/* {{{ facedetect_functions[]
 *
 * Every user visible function must have an entry in facedetect_functions[].
 */
static zend_function_entry facedetect_functions[] = {
    PHP_FE(face_detect, NULL)
    PHP_FE(face_count, NULL)
    {NULL, NULL, NULL}
};
/* }}} */

/* {{{ facedetect_module_entry
 */
zend_module_entry facedetect_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    PHP_FACEDETECT_EXTNAME,
    facedetect_functions,
    NULL,
    NULL,
    NULL,
    NULL,
    PHP_MINFO(facedetect),
#if ZEND_MODULE_API_NO >= 20010901
    PHP_FACEDETECT_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_FACEDETECT
ZEND_GET_MODULE(facedetect)
#endif

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINFO_FUNCTION(facedetect)
{
	//php_info_print_table_start();
	//php_info_print_table_row(2, "facedetect support", "enabled");
	//php_info_print_table_row(2, "facedetect version", PHP_FACEDETECT_VERSION);
	//php_info_print_table_row(2, "OpenCV version", CV_VERSION);
	//php_info_print_table_end();
}
/* }}} */

static void php_facedetect(INTERNAL_FUNCTION_PARAMETERS, int return_type)
{

	char *file, *casc;
	long flen, clen, fcn_len, ecn_len;
	bool use_eye = true;

	zval *array, *array_eyes, *return_value_eyes;

    Mat gray;
	//Mat *img;    faces;
	std::vector<Rect> faces, *yes;
	CvRect *rect;

    char *fcn;// = "haarcascade_frontalface_alt.xml";
    char *ecn;// = "haarcascade_eye.xml";

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|ssb", &file, &flen, &fcn, &fcn_len, &ecn, &ecn_len, &use_eye) == FAILURE) {
		RETURN_NULL();
	}

    IplImage* iplimg = cvLoadImage(file);
    Mat img(iplimg, false);

	if(!img.data) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Image not load!");
		RETURN_FALSE;
	}

    if( !face_cascade.load( fcn ) ){
        php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Can't load face cascade file!");
        RETURN_FALSE;
    };

    if( !eyes_cascade.load( ecn ) ){
        php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Can't load eyes cascade file!");
        RETURN_FALSE;
    };

    //gray = cvCreateImage(cvSize(img->width, img->height), 8, 1);
	cvtColor(img, gray, CV_BGR2GRAY);
    equalizeHist( gray, gray );

    face_cascade.detectMultiScale( gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );
    int j, i;
    array_init(return_value);
    for( i = 0; i < faces.size(); i++ )
    {

        Mat faceROI = gray( faces[i] );
        std::vector<Rect> eyes;

        eyes_cascade.detectMultiScale( faceROI, eyes, 1.1, 2, 0 |CV_HAAR_SCALE_IMAGE, Size(30, 30) );
        if (eyes.size() > 0 || !use_eye) {
            MAKE_STD_ZVAL(array);
            array_init(array);

            add_assoc_long(array, "x", faces[i].x);
            add_assoc_long(array, "y", faces[i].y);
            add_assoc_long(array, "w", faces[i].width);
            add_assoc_long(array, "h", faces[i].height);


            MAKE_STD_ZVAL(return_value_eyes);
            array_init(return_value_eyes);

            for( j = 0; j < eyes.size(); j++ )
            {
                 MAKE_STD_ZVAL(array_eyes);
                 array_init(array_eyes);
                 add_assoc_long(array_eyes, "x", eyes[j].x);
                 add_assoc_long(array_eyes, "y", eyes[j].y);
                 add_assoc_long(array_eyes, "w", eyes[j].width);
                 add_assoc_long(array_eyes, "h", eyes[j].height);
                 add_next_index_zval(return_value_eyes, array_eyes);
            }

            add_assoc_zval(array, "eyes", return_value_eyes);
            add_next_index_zval(return_value, array);
        }
    }
}

/* {{{ proto array face_detect(string picture_path, string cascade_file, string eye_cascade_file)
   Finds coordinates of faces (or in gernal "objects") on the given picture */
PHP_FUNCTION(face_detect)
{
	php_facedetect(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto int face_count(string picture_path, string cascade_file)
   Finds number of faces (or in gernal "objects") on the given picture*/
PHP_FUNCTION(face_count)
{
	php_facedetect(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

