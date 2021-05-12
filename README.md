# CW2_GLSL
 
Visual Studio 2019 and Operating System windows 10

----------------------------------------------------------
How to use
----------------------------------------------------------

The executable is a Optimised Developer Tool prototype to show off my knowledge of GLSL. 

When the executable is run, with some statues and 2 fire chalace.

This Developer Tool is a simple day cycle with the input being bound to the number keys on a keyboard.

Each number key is bound to a light position (time of day) as shown below:

![](media/sense.png)

----------------------------------------------------------
How does it work / how does the code fit together
----------------------------------------------------------

When the program starts up it compiles all of the shaders that it will need to use.
For this Developer Tool 5 shaders are used these shaders are:

Shader one (basic_uniform) is used to create the 4 statues and the corresponding lights
The shader starts off by creating the textures that will be rendered on the models for the
floor. It is fairly straight forward, it just misses the ground texture with a water texture.
However, for the statue texture it gets a bit more complicated as the model not only uses the
statue texture and the moss texures mixed together but it also uses a generated noise texture 
with a colour of green to add a darker looking moss to all of the statues.

The second shader (normal) is used for the fire chalace within the scene. This shader implements PBR shading on the models to give them a more realistic texture and light mapping.
It does this by calculating microfacet in the surface of the models and uses these in approximating the reflections of the lights. It also takes in a normal mapped texture and mixes this into the texture of the model.

The third shader (skybox) is used to create a skybox around the scene and it does this by taking in 6 textures and then cube mapping them together.

Shaders 4 and 5 are almost identical. These shaders are used to create the partical effect for the fire and smoke within the scene. For this to be done they instantiate a partical and assign it a velocity age direction and start position. Once the partical is emmited from its position its colour slowly becomes darker and it starts to fade the longer it has been alive until it disappears when it hits its life time.

--------------------------------------------------------------------------------------------
What makes your shader program special and how does it compare to similar things?
--------------------------------------------------------------------------------------------

This project started with the prototype used in GLSL_cw1 in which it used a model. You could
move around withn WASD and had 3 lights and a spotlight (which was not implemented correctly and therefore emmited no light).

This development tool prototype is diffrent to similiar ones due to the light positions being binded to the number keys and not updated through the update() function. This was done so as to be able to get a better view of what the light was doing at that postiion and not have it constantly moving.

Also this shader is special due to the partical shader also being bound to the keys. So when the light is lower the fire starts up but when the light is higher in the sky, the fire is put out and smoke starts billowing out instead, showing that the fire has been put out.
