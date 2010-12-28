import java.lang.System;

public class Timer
{
      long startTime;
      
      public void start() {
	    startTime = System.nanoTime();	
      }
      
      public long getElapsedNano() {
	    return (System.nanoTime() - startTime);
      }
      
      public double getElapsedSecs() {
	    return getElapsedNano() / 1000000000.0;
      }
      
      public double getBandwidth(long size) {
    	    return size / getElapsedSecs();
      }
}
