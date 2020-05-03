ESCAPE - закрытие окна
Q - включить/выключить каркасную симуляцию воды
RIGHT_MOUSE_BUTTON - создать возмущение на поверхности воды
(база 30 баллов)

компиляция:
g++ main.cpp maths.cpp util.cpp rectangle.cpp water_surface.cpp plane.cpp shader.cpp -I. -lGL -lGLEW -lglfw -lm

используемые материалы:
1) доклад Matthias Muller-Fischer - "Fast Water Simulation for Games Using Height Fields" https://ia801906.us.archive.org/12/items/GDC2008Fischer/GDC2008-Fischer.pdf
2) Eric Lengyel, Mathematics for 3D Game Programming and Computer Graphics, Third Edition
