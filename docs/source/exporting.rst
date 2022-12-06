Exporting
=========

Commands for exporting are located under the **File** menu.

- Exporting the Display as a Bitmap
- Exporting an Animation

.. - Printing TODO

"""""""""""""""""""""""""""""""""
Exporting the Display as a Bitmap
"""""""""""""""""""""""""""""""""

MvMf6 can export a bitmap of the display area.

#. Resize the display area to the desired dimensions.
#. Click the **File** menu and select **Export As Bmp**. (See note 1 below.)
#. Select **Screen** if you want a bitmap at the screen resolution. Otherwise, select **150 ppi** or **300 ppi** and specify either the **width** or **height** (but not both) of the bitmap in inches. (See note 2 below.)
#. Click **OK**.
#. Navigate to the folder in which you want to save the bitmap file.
#. In the **File Name** box, enter the name of the bitmap file. This name should end with the extension **.bmp**
#. Click Save.

.. note::
    #. The exported bitmap has a color depth of 24 bits per pixel, or a maximum of about 16 million colors.
    #. If you select **150 ppi** or **300 ppi** (pixels per inch) in Step 2, the bitmap size will depend on the specified **width** or **height** in inches. If you specify the **width**, then the height will be automatically determined so that the aspect ratio of the bitmap matches that of the display area. If you specify **height**, then the width will be automatically determined. For example, if you specify 300 ppi and a width of 4 inches, the bitmap will have 1200 pixels in the x dimension. When you import such a bitmap into an application that recognizes bitmap resolution, the bitmap will be displayed as a picture with the specified width and height in inches.


""""""""""""""""""""""
Exporting an Animation
""""""""""""""""""""""

MvMf6 can export a sequence of bitmap files, one for each time when results are saved. The saved bitmaps files may be used to create an animation (for example, mpeg, animated gif) using an application that creates animations. (See Note 1 below)

#. Resize the display area to the desired dimensions. (See Note 2 below)
#. Click the **File** menu and select **Export Animation**.
#. Set the **Animation type** to "time" or "viewpoint."
#. If the **Animation type** is set to "viewpoint", set the **Number of steps**.
#. If the **Animation type** is set to "time", specify the time span in the **From** and **To** boxes.
#. In the **Rotate box**, enter the amount of viewpoint rotation (in degrees) between successive times. (See Note 3 below)
#. In the **Elevate** box, enter the amount of viewpoint elevation (in degrees) between successive times. (See Note 3 below)
#. Enter the **File Prefix** and the **Start Number**. For example, if you enter **sim** for File Prefix and **001** for Start Number, then the files will be named sim001.bmp, sim002.bmp, sim003.bmp, etc.
#. Click the **Browse** button and navigate to the folder in which you want to save the files. Click the folder and then click **Select**.
#. To preview the animation that will be saved, click **Preview**. The program will display the sequence of images to be saved, but will not save them.
#. To save the animation, click **Export**. A sequence of bitmap files are created.
#. Click **Save**.

.. note::
    #. The bitmaps are saved at the screen resolution and have the same size as the display area. The color depth is 24 bits per pixel, or a maximum of about 16 million colors.
    #. The saved bitmap files are not compressed and might occupy significant disk space. To conserve disk space, resize the display area to a relatively small size before saving the sequence of bitmaps.
    #. If you want a stationary viewpoint for the animation, enter 0 in both the **Rotate** and **Elevate** boxes.
