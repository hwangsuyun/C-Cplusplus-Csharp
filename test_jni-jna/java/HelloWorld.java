import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Platform;
import com.sun.jna.Callback;
import com.sun.jna.Pointer;


public class HelloWorld {
    public interface CLibrary extends Library {
        public interface NotificationListener extends Callback {
            void invoke(Pointer val, int length);
        }
        
        public static class NotificationListenerImpl implements NotificationListener {
            @Override
            public void invoke(Pointer val, int length) {
                System.out.println("length : " + length);
                System.out.println("callback :  + new String(val.getByteArray(0, length)));
        }
        
        CLibrary INSTANCE = (CLibrary) Native.load(("./myLib.so"), CLibrary.class);
        void callbackTrigger(NotificationListener callback);
		void printHello();
		void printString(String str);
    }

    public static void main(String[] args) {
        CLibrary.NotificationListenerImpl callbackImpl = new CLibrary.NotificationListenerImpl();
		CLibrary.INSTANCE.printHello();
		int result = CLibrary.INSTANCE.printString("HI\n");
		System.out.println("result : " + result);
        CLibrary.INSTANCE.callbackTrigger(callbackImpl);
    }
}