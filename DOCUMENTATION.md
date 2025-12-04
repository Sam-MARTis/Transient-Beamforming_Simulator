# Docs
Honestly, this is more for me to keep track of what the hell I was even thinking while I made the functions so I can take breaks and not be completely lost when I come back.



### Display functions
1. The end sim should have options of displaying the pressure, divergence, etc of the cells.
2. It should also have an option of borders between cells, atleast for debugging. I'll set thickness to zero later.
3. It should be able to render the wall velocities, so `hvels` and `vvels` so that they are attached to the cell.
4. It should also be able to render a flow field so I can see verify that interpolation is working.
5. It should be able to render moving particles like smoke or some beads. 


5 we can work on once the sim works. 
For 1, I am planning to have a `display_shapes` function that takes in the sfml shapes and a pointer to the property that needs to be displayed. 
Since different properties need different normalization factors, I will pass in a property range as well. 
```
display_shapes(rendering_window, shapes, dimensions, properties_array, property_min, property_max)
```


The GUI has a dropdown menu for choosing values like pressure, divergence, constant colour, etc.
The property choosen is then used in the main.cpp to choose the appropriate property of the fluid sim container structure that contains all the data. This chosen property (an array pointer) is passed into the `display_shapes` function



## Mistakes I made

### Shitty Macros
I made a linear interpolation macro for finding velocity at any arbitrary point. Was meant to be used for bilinear iinterpolation
```cpp
#define INTERPOLATE(a, b, frac) (((1.0f - (float)frac) * ((float)a) + ((float)frac) * ((float)b)))
```
Can you spot an issue?
If yes, screw you and your good eyes.
If not, me too.

This is how I was using it:
```cpp
 hvel_1 = HVELS(i, j, nx, ny);
hvel_2 = HVELS(i + 1, j, nx, ny);
hvel_3 = HVELS(i, j + 1, nx, ny);
hvel_4 = HVELS(i + 1, j + 1, nx, ny); 
hvel_top = INTERPOLATE(hvel_1, hvel_2, i_frac);
hvel_bottom = INTERPOLATE(hvel_3, hvel_4, i_frac);
hvel = INTERPOLATE(hvel_top, hvel_bottom, j_frac - 0.5f);
```

And when I tried debugging: 
```cpp
hvel_top = INTERPOLATE(hvel_1, hvel_2, i_frac);
std::cout << "hvel_top: " << hvel_top << "\n"; 
hvel_bottom = INTERPOLATE(hvel_3, hvel_4, i_frac); 
std::cout << "hvel_bottom: " << hvel_bottom << "\n"; 
hvel = INTERPOLATE(hvel_top, hvel_bottom, j_frac - 0.5f); 
std::cout << "j_frac - 0.5f: " << j_frac - 0.5f << "\n"; 
std::cout << "Interpolate(" << hvel_top << ", " << hvel_bottom << ", " << j_frac - 0.5f << "): " << INTERPOLATE(hvel_top, hvel_bottom, j_frac - 0.5f) << "\n"; std::cout << "HVel: " << hvel << "\n\n";
```

The output I got was this: 
```bash
hvel_top: 0.634808 
hvel_bottom: 0 
j_frac - 0.5f: 0.0799999 
Interpolate(0.634808, 0, 0.0799999): -0.0507846 
HVel: -0.0507846
```

Notice this line:
```bash
Interpolate(0.634808, 0, 0.0799999): -0.0507846 
```

INTERESTING, IS IT NOT?!
KNOW THE PROBLEM?
MACRO EXPANSION! The `j_frac-0.5f` is not evaluated and is directly expanded.
When I have `1-frac` in the macro,  it becomes `1-j_frac-0.5f` instead of `1-(j_frac-0.5f)`
...yeah
Putting brackers in the macro around frac solved it.

Corrected macro:
```cpp
#define INTERPOLATE(a, b, frac) (((1.0f - ((float)(frac))) * ((float)a) + ((float)(frac)) * ((float)b)))
```

The amount of time I spent on this...




