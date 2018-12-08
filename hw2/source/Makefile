rasterizer_cpp:
	g++ -o rasterizer hw2_math_ops.cpp hw2_file_ops.cpp rasterizer.cpp 

run:
<<<<<<< HEAD
	./rasterizer ../inputs/culling_enabled_inputs/horse_and_mug/horse_and_mug_scene.txt ../inputs/culling_enabled_inputs/horse_and_mug/horse_and_mug_camera.txt
=======
	./rasterizer ../inputs/culling_disabled_inputs/filled_box/filled_box_scene.txt ../inputs/culling_disabled_inputs/filled_box/filled_box_camera.txt
	
run_all:
	for dir in ../inputs/* ; do \
		for input in $$dir/*; do \
			input_name=$$(echo $$input | rev | cut -d'/' -f1 | rev) ; \
			scene_file=$$input/$$input_name"_scene.txt" ; \
			camera_file=$$input/$$input_name"_camera.txt" ; \
			output_dir=$$(echo $$input | cut -d'/' -f2- | sed "s/input/output/g")  ; \
			mkdir -p $$output_dir ; \
			./rasterizer $$scene_file $$camera_file ; \
			mv *.ppm $$output_dir ; \
		done \
	done
>>>>>>> 1a331c80812e2d776a93b02162b78b67ab558a99

clean:
	rm -rf *.ppm *.png ./rasterizer outputs/
