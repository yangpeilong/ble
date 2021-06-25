package com.qdingnet.provider.config.upgrade;

import android.util.Log;

import org.apache.http.ConnectionClosedException;
import org.apache.http.HttpException;
import org.apache.http.HttpServerConnection;
import org.apache.http.protocol.BasicHttpContext;
import org.apache.http.protocol.HttpContext;
import org.apache.http.protocol.HttpService;

import java.io.IOException;

public class WorkerThread extends Thread {
	private final static String TAG = "WorkerThread";

	private final HttpService httpservice;
	private final HttpServerConnection conn;

	public WorkerThread(final HttpService httpservice, final HttpServerConnection conn) {
		super();
		this.httpservice = httpservice;
		this.conn = conn;
	}

	@Override
	public void run() {
		HttpContext context = new BasicHttpContext();
		try {
			while (!Thread.interrupted() && this.conn.isOpen()) {
				this.httpservice.handleRequest(this.conn, context);
			}
		} catch (ConnectionClosedException ex) {
			Log.e(TAG,"Client closed connection",ex);
		} catch (IOException ex) {
			Log.e(TAG,"I/O error: " ,ex);
		} catch (HttpException ex) {
			Log.e(TAG,"Unrecoverable HTTP protocol violation: ",ex);
		} finally {
			try {
				this.conn.shutdown();
			} catch (IOException ignore) {
			}
		}
	}

}
