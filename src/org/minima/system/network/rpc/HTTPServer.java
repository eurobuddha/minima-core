package org.minima.system.network.rpc;

import java.io.IOException;
import java.net.BindException;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;

import org.minima.utils.MinimaLogger;

public abstract class HTTPServer extends Server implements Runnable{

	ServerSocket mServerSocket;
	
	/** "" = every interface; else the one address to bind (MDS uses GeneralParams.MDS_BIND_HOST) */
	String mBindHost = "";
	
	boolean mRunning = true;
	
	/** Why the server could not start (bind failed, unknown host..), or null */
	volatile String mStartError = null;
	
	public HTTPServer(int zPort) {
		this(zPort, true);
	}
	
	public HTTPServer(int zPort, String zBindHost) {
		this(zPort, zBindHost, true);
	}
	
	public HTTPServer(int zPort, boolean zAutoStart) {
		this(zPort, "", zAutoStart);
	}
	
	public HTTPServer(int zPort, String zBindHost, boolean zAutoStart) {
		super(zPort);
		mBindHost = zBindHost == null ? "" : zBindHost;
		
		if(zAutoStart) {
			start();
		}
	}
	
	@Override
	public void shutdown() {
		mRunning = false;
		
		try {
			if(mServerSocket != null) {
				mServerSocket.close();
			}
		} catch (Exception e) {
			MinimaLogger.log(e);
		}
	}
	
	public void start() {
		Thread runner = new Thread(this);
		runner.start();
	}
	
	public abstract Runnable getSocketHandler(Socket zSocket);
	
	@Override
	public void run() {
		try {
			//Start a server Socket..
			if(mBindHost == null || mBindHost.isEmpty()) {
				mServerSocket = new ServerSocket(mPort);
			}else {
				mServerSocket = new ServerSocket(mPort, 50, InetAddress.getByName(mBindHost));
			}
			
			MinimaLogger.log("Server started on "+(mBindHost == null || mBindHost.isEmpty() ? "*" : mBindHost)+":"+mPort);
			
			//Keep listening..
			while(mRunning) {
				//Listen in for connections
				Socket clientsock = mServerSocket.accept();
				
				//Get the Handler..
				Runnable handler = getSocketHandler(clientsock);
				
				//Run in a new Thread
				Thread rpcthread = new Thread(handler, "Socket Handler @ "+getPort());
				rpcthread.setDaemon(true);
				rpcthread.start();
			}
			
		} catch (BindException e) {
			//Could not bind - port in use, or an address this machine does not have
			mStartError = "Cannot listen on "+(mBindHost == null || mBindHost.isEmpty() ? "*" : mBindHost)+":"+mPort+" - "+e.getMessage();
			MinimaLogger.log("Server @ Port "+mPort+" not started: "+mStartError);
			
		} catch (SocketException e) {
			if(mRunning) {
				//Socket shut down..
				MinimaLogger.log("RPCServer : Socket Shutdown.. "+e);
			}
		} catch (IOException e) {
			if(mServerSocket == null) {
				mStartError = "Cannot listen on "+mBindHost+":"+mPort+" - "+e;
			}
			MinimaLogger.log(e);
		}
	}
	
	/** Is the server socket open and accepting? */
	public boolean isListening() {
		return mServerSocket != null && !mServerSocket.isClosed();
	}
	
	/** Why the server did not start, or null */
	public String getStartError() {
		return mStartError;
	}
}