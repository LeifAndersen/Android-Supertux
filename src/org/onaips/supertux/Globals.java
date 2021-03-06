// This string is autogenerated by ChangeAppSettings.sh, do not change spaces amount anywhere
package org.onaips.supertux;


class Globals {
	public static String ApplicationName = "SuperTux";

	// Should be zip file
	public static String DataDownloadUrl = "http://fe.up.pt/~ee06201/supertux2_0.2.zip"; // This string is autogenerated by ChangeAppSettings.sh, do not change spaces amount

	public static boolean DownloadToSdcard = true;
	
	// Set this value to true if you're planning to render 3D using OpenGL - it eats some GFX resources, so disabled for 2D
	public static boolean NeedDepthBuffer = false;

	public static boolean HorizontalOrientation = true;
	
	// Readme text to be shown on download page
	public static String ReadmeText = "^Wait while Supertux downloads data files, this only occurs once.^You can press home, the download will continue.^^Do NOT turn screen off or your connection will end & you'll have to download again.^".replace("^","\n");
	
	public static boolean AppUsesMouse = true;

	// We have to use accelerometer as arrow keys
	public static boolean AppNeedsArrowKeys = true;
	
	public static boolean AppUsesJoystick = false;

	public static boolean PhoneHasTrackball = true;
}

class LoadLibrary {
	public LoadLibrary() { 
	System.loadLibrary("sdl"); 
	System.loadLibrary("openal"); 
	System.loadLibrary("physfs");
	System.loadLibrary("mad");
	System.loadLibrary("sdl_image");
	System.loadLibrary("iconv");
	System.loadLibrary("boost");
	System.loadLibrary("curl");
	};
}
