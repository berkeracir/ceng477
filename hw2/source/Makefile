rasterizer_cpp:
	g++ -o rasterizer hw2_math_ops.cpp hw2_file_ops.cpp rasterizer.cpp 
	
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

clean:
	rm -rf *.ppm *.png ./rasterizer outputs/
