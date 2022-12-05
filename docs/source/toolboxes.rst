Toolboxes
=========

MvMf6 provides a number of small dialog boxes, called **toolboxes**, for controlling specific aspects of the display. 

- The **Toolbox** menu provides commands to show or hide the toolboxes. 
- A check mark next to a command indicates that the corresponding toolbox is shown. Selecting this command will hide the toolbox. 
- The absence of a check mark next to a command indicates that the corresponding toolbox is hidden. Selecting this command will show the toolbox. 
- Each toolbox may be closed by clicking **Done**. 
- Toolboxes do not have to be closed for the program to continue. 
- Except for text entries, all controls on toolboxes will take effect immediately when changed.
- For text entries, the user must click **Apply** to apply the change. 

""""""""""""
Data Toolbox
""""""""""""

The **Data** toolbox displays the maximum and minimum data values at the current time, and enables you to select the type of scalar data to display.

- If the **Data** toolbox is not visible, display it by clicking the **Toolbox** menu and selecting **Data**. 
- The **Scalar** tab displays the minimum and maximum scalar values at the current time. You may also select the type of scalar data to display from the drop-list box. 
- The **Vector** tab displays the minimum and maximum magnitude of vectors at the current time. 
- The **Pathline** tab displays the minimum and maximum travel times for all pathlines. 

"""""""""""""""""
Color Bar Toolbox
"""""""""""""""""

The **Color Bar** toolbox controls the properties of the color bar.

- If the **Color Bar** toolbox is not visible, display it by clicking the **Toolbox** menu and selecting **Color Bar**.
- Clicking **Default** will assign default settings to the selected tab. Default settings are given in parentheses below.
- The **Source** tab allows you to specify whether the color bar displays data for the active data set or the pathlines.
- The **Limits** tab allows you to: 

  * Set the scalar value at the blue and red limits. The default settings are: 

    - blue = minimum scalar value at the current time
    - red = maximum scalar value at the current time 

  * Specify a linear color bar by unchecking the **logarithmic scale** option. (This is the default setting.)
  * Specify a logarithmic color bar by checking the **logarithmic scale** option.
  * Interchange the scalar values at the blue and red limits by clicking **Reverse**.

- The **Size** tab allows you to: 

  * Set the **width** of the color bar in pixels. (20)
  * Set the **height** of the color bar in pixels. (200)
  * Set the **offset** distance in pixels from the right edge of the color bar to the right side of the display area. (100)

- The **Labels** tab allows you to: 

  * Set the **font size** (in points) of the labels. (14) 
  * Set the **number of labels**. (5) 
  * Set the **precision** (number of significant places) of the labels. (2) 
  * Set the label color to black, gray, or white. (black) 

- The **Colors** tab allows you to: 

  * Select the Default color scheme 
  * Select the Modified color scheme which is similar to the default except that the thickness of the yellow and aqua portions of the color bar have been increased at the expense of the green, red, and blue portions of the color bar. 
  * Select a Custom color scheme in which the beginning and ending colors are selected by the user. 

If the Custom color scheme is selected, the user clicks the **Choose First Color** or **Choose Last Color** buttons to choose the colors of the endpoints of the color bar. 

- The Crop tab allows you to: 

  * Specify the **Min** and **Max** positions of the x, y, and z cropping planes. (Min = 0, Max = 1) 
  * Specify the **Delta** value, which is used to increase or decrease the Min or Max position when the up or down arrow is clicked. (0.1) 
  * Specify the **Crop Angle** to rotate the x and y cropping planes. The crop angle can be from -45 to 45 degrees. (0) 


""""""""""""""""
Lighting Toolbox
""""""""""""""""

The **Lighting** toolbox allows adjustment of how graphic objects are illuminated. The default lighting provided by MvMf6 is a **headlight**, which illuminates objects from the direction of the viewpoint. *The default setting is adequate for most cases, and lighting adjustment is often unnecessary*. 

- If the **Lighting** toolbox is not visible, display it by clicking the **Toolbox** menu and selecting **Lighting**. 
- Clicking **Default** will assign default settings to the selected tab. Default settings are given in parentheses below. 
- The **Lights** tab allows you to: 

  * Turn on the headlight by checking the **Headlight** option. (By default, the headlight is turned on.) 
  * Turn off the headlight by unchecking the **Headlight** option. 
  * Set the headlight intensity. (1) 
  * Turn on the auxiliary light by checking the **Auxiliary Light** option. 
  * Turn off the auxiliary light by unchecking the **Auxiliary Light** option. (By default, the auxiliary light is turned off.) 
  * Set the auxiliary light intensity. (0) 
  * Set the vector direction from which the auxiliary light illuminates graphic objects. The vector components (x, y, z) may vary from -1 to 1. For example, if x= 0, y= 0, z= 1, the auxiliary light will shine from the positive z axis towards the negativez axis.

- The **Surface** tab allows you to set the surface properties of graphic objects. These properties are:

  * Diffuse: This parameters is currently not adjustable and is set to 1.0.
  * Ambient: Increasing this parameter above 0 gives surfaces a "washed out" appearance and reduces the shading. Setting this parameter to the maximum value of 1 causes surfaces to appear white. (0)
  * Specular: The amount of reflected light. (0)
  * Specular Power: The surface shininess. (1)

- The **Background** tab allows you to:
  * Set the background color to white by selecting the **White** option. (White is the default background color.)
  * Set the background to any color by selecting the **Custom** option and specifying the red, green, and blue components of the color.
