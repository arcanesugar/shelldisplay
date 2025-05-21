Imagine a shell, now imagine things displayed in it

Every function is included in shelldisplay.h

Textures:
Textures are stored as a struct that contains a buffer of Colors.\
They must be allocated and freed with allocTexture() and freeTexture(), but the Texture struct itself can exist on the stack.\
Textures can be resized using resampleTexture() which returns a new texture that must also be freed. The original texture is kept intact

### void sampleTexture(Texture t, double x, double y, int method);


### void printTexture(Texture t, int pixelType)
prints the given texture to stdout. The color accuracy of the printed texture will depend on the shell and the current colorMode().\
__Pixel Types__\
1 character is twice as tall as it is wide, so a square pixel's width must be double it's height\
Full block characters are used to represent spaces
enum|size|char|notes
---|---|---|---
PIX_DOUBLE_SPACE|2x1|██| Square but large
PIX_SINGLE_SPACE|1x1|█| Rectangular but allows for higher resolution. <br> Note that the texture must be wider than it is tall to remain square when printed
PIX_UPPER_HALF_BLOCK|1x0.5|▀| Small square pixels. <br> Doubles the resolution of double space but requires unicode
