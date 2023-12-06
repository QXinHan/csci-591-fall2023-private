# README

## Target

This code is used to test , the if the conversion of  `.png -> .txt -> .png` works or not.

 ## Question

The code works successfully. I read the file `testPNG.png` and write the data of it into two files, `testPNG_write.txt` and `testPNG_write.png`.

**The `.txt` file:**

![capture_20231206163657022](D:\gitspace\csci-591-fall2023-private\PNGConvertion\capture_20231206163657022.bmp)

**In utf-8 it is:**

![capture_20231206163948491](D:\gitspace\csci-591-fall2023-private\PNGConvertion\capture_20231206163948491.bmp)

The binary data of the two files is the same, But when I modify the extension name(`.txt`) of the two files to `.png` respectively. The first one works correctly and I get the image, but the second one fails. **I think the problem is caused by the difference of the encoding(UTF-8 and ANSI), just a guess and don't know why.**

The data in the second one  is binary. I open the `testPNG.png` by WinHex and copy the binary data to the `text.txt`.

The data in the second screenshot is the binary code for the image, I write a program to read them and and write them into a file.

