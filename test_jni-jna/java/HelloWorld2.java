import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Platform;
import com.sun.jna.Callback;
import com.sun.jna.Pointer;


public class HelloWorld {
    public interface CLibrary extends Library {
        public interface EventCallback extends Callback {
            void invoke(Pointer val, int length);
        }
        
        public static class EventCallbackImpl implements EventCallback {
            @Override
            public void invoke(Pointer val, int length) {
                System.out.println("length : " + length);
                System.out.println("callback :  + new String(val.getByteArray(0, length)));
        }
        
        CLibrary INSTANCE = (CLibrary) Native.load(("./myLib.so"), CLibrary.class);
        void callbackTrigger(EventCallback callback);
		void printHello();
		void printString(String str);
    }

    public static void main(String[] args) {
        CLibrary.EventCallbackImpl callbackImpl = new CLibrary.EventCallbackImpl();
		
		Thread loopThread = new Thread(() -> {
			CLibrary.INSTANCE.printHello();
			int result = CLibrary.INSTANCE.printString("HI\n");
			System.out.println("result : " + result);
		});
		loopThread.start();
        
		try {
			while (true) {
				CLibrary.INSTANCE.callbackTrigger(callbackImpl);
				System.out.println("infinite...");
				Thread.sleep(1000);
			} catch (InterruptedException e) {
			}
		}
		
		
    }
}