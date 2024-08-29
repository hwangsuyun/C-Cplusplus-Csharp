import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Platform;

public class HelloWorld {
    public static void main(String[] args) {
        CLibrary.INSTANCE.printHello();
        int result = CLibrary.INSTANCE.printString("HI\n");
	System.out.println("result : " + result);
    }
}

interface CLibrary extends Library {
    CLibrary INSTANCE = (CLibrary) Native.load(
        ("/SIEM/ExdCollector/sg/jna/myLib.so"), CLibrary.class);
        void printHello();
        int printString(String str);
}
