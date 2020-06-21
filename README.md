# VFDhack
VFDhack is the project that I tried to code the library for MN15439A. More Info on my blog https://tinlethax.wordpress.com/2020/05/01/six-years-of-finding-the-datasheet-and-pinout-of-the-mn15439a/

# Datasheet

Unfortunately, There's no datasheet for this VFD Display but as far as I know

1. Manufactured by Noritake Itron. 
2. It's Chip In Glass Vacuum Fluorescent Display 
3. It used to be the main Display in CDX-GT700D. Car stereo CD player made by SONY way back in 2005s.
4. This display is custom-made. SONY signed Non-Disclosure-Agreement (NDA) with Noritake Itron. So no full datasheet available D:


But there's one of the display that close enought to be the manufacturing baseline for the MN15439A. It's MN14440A 

# BUT I DO HAVE PINOUT 

![Pinout from CDX-GT700D service manual](https://tinlethax.files.wordpress.com/2020/05/service_manual3.png)
![Actual thing](https://3g4zpw.bn.files.1drv.com/y4mKyH7eJ1yTGaRiTI9MW2V7h5LcOD4xVoQQt-13Q081j3SKQBJp4DOd7w8G_lDZ-pH3jxa8YsHAq4-LBMg2eRPlq-IhBheBpS8xwrEd6YQSQQb7UJb5Qw8E28-Sxf3cA0m7YeL83z7kj8P6CozIrhz2_qBiEoNS8UYaAitQItRswdgqQPk_DwOevwMEVuqrEm9JG3GOxp0w-G_yyGVJ3EzLA?width=5888&height=3312&cropmode=none)

# Source code 

The source code only contain the bit banging of 286 bit datastream, but not comfirmed that it's working.

# Hardware

I already Designed the board and Assembled about 30-40% (Most component still on the way comung from China. Due to COVID-19, it might kes upto 3 months 
