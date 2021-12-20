#include "stereokit.h"
#include "stereokit_ui.h"
using namespace sk;

#include <stdio.h>

#include <opencv2/opencv.hpp>
#include <unistd.h>

struct threaded_image_manager
{
	pthread_mutex_t mutex;
	pthread_t thread;
	bool reader_should_stop;
	cv::Mat *front_mat;
	float width_over_height;
};

struct threaded_image_manager *our_man;

sk::model_t img_model;
sk::material_t img_material;
sk::tex_t img_tex;

void *
cv_loop(void *manager_ptr)
{
	threaded_image_manager *man = (threaded_image_manager *)manager_ptr;

  // Pick the first camera we can find - probably /dev/video0
	cv::VideoCapture cap(0);
	
  // Use MJPG format if it's available! This greatly reduces the bandwidth requirements, especially on USB2
	// cameras
	cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G')); // MJPEG

	// Set to 60fps. For cameras with lower max FPS values, it'll just pick the maximum!
	cap.set(cv::CAP_PROP_FPS, 60);

	cv::Mat img_bgr;
	cv::Mat img_rgba;

	cv::Mat img_1;
	cv::Mat img_2;

	cv::Mat *back_mat = &img_1;
	cv::Mat *front_mat = &img_2;

	while (!man->reader_should_stop) {
		cap >> img_bgr;
		if (img_bgr.empty()) {
			printf("no image yet\n");
			usleep(10000);
			continue;
		}

		std::vector<cv::Mat> channels;

		cv::split(img_bgr, channels);

		cv::Mat alpha(img_bgr.rows, img_bgr.cols, CV_8U, cv::Scalar(255));

		std::vector<cv::Mat> argb = {channels[2], channels[1], channels[0], alpha};

		cv::merge(argb, *back_mat);

		// Critical section - lock the mutex
		pthread_mutex_lock(&man->mutex);
		// Swap the buffers - if we wrote into img_1, next time we will write into
		// img_2. Also, point man->front to the mat we just wrote into
		man->front_mat = back_mat;
		back_mat = front_mat;
		front_mat = man->front_mat;

		man->width_over_height = (float)man->front_mat->cols / (float)man->front_mat->rows;

		// End of critical section; unlock
		pthread_mutex_unlock(&man->mutex);
	}
	cap.release();
	return NULL;
}

void
update()
{

	if (pthread_mutex_trylock(&our_man->mutex) == 0) {

		if (our_man->front_mat != NULL) {
			tex_set_colors(img_tex, our_man->front_mat->cols, our_man->front_mat->rows,
			               our_man->front_mat->data);

			material_set_texture(img_material, "diffuse", img_tex);
		}
		pthread_mutex_unlock(&our_man->mutex);
	}
	float height = 0.5f;
	// 0.5 meters in front of your head's starting position
	sk::vec3 offset = {0, 0, -0.5};
	// Width, height, 1
	sk::vec3 scale = {our_man->width_over_height * height, height, 1};

	sk::render_add_model(img_model, matrix_trs(offset, sk::quat_identity, scale));
}

int
main()
{
	sk_settings_t settings = {};
	settings.app_name = "StereoKit webcam demo!";
	settings.display_preference = display_mode_mixedreality;
	if (!sk_init(settings))
		return 1;

	sk::mesh_t m = sk::mesh_gen_plane({1, 1}, {0, 0, 1}, {0, 1, 0});

	img_tex = tex_create(tex_type_image, tex_format_rgba32);

	img_material = material_copy_id("default/material");

	material_set_float(img_material, "tex_scale", 1);
	material_set_cull(img_material, sk::cull_none);

	img_model = model_create_mesh(m, img_material);

	our_man = (threaded_image_manager *)malloc(sizeof(threaded_image_manager));
	pthread_mutex_init(&our_man->mutex, NULL);
	our_man->reader_should_stop = false;
	our_man->width_over_height = 1;
	our_man->front_mat = NULL;
	pthread_create(&our_man->thread, NULL, cv_loop, our_man);

	while (sk_step(update)) {
		// Do nothing! update did everything already!
	};

	sk_shutdown();
	return 0;
}
