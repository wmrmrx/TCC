run:
	manim -pql main.py Draw < ./input

build_and_run_random: 
	g++-14 ../scripts/test_generator.cpp -o ../scripts/test_generator
	../scripts/test_generator 20 > ../tests/input
	python3.13 main.py < ../tests/input

test:
	pytest -v -s increment.py