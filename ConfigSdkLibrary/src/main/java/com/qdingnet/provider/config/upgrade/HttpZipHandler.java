package com.qdingnet.provider.config.upgrade;


import com.qdingnet.provider.config.Logdeal;

import org.apache.http.HttpEntity;
import org.apache.http.HttpException;
import org.apache.http.HttpRequest;
import org.apache.http.HttpResponse;
import org.apache.http.HttpStatus;
import org.apache.http.entity.ContentProducer;
import org.apache.http.entity.EntityTemplate;
import org.apache.http.entity.StringEntity;
import org.apache.http.protocol.HttpContext;
import org.apache.http.protocol.HttpRequestHandler;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;
import java.util.zip.ZipEntry;
import java.util.zip.ZipOutputStream;

public class HttpZipHandler implements HttpRequestHandler {
	private final static String TAG = "HttpZipHandler";
	/** 缓冲字节长度1M=1024*1024B */
	private static final int BUFFER_LENGTH = 1048576;

	protected String webRoot;

	public HttpZipHandler(final String webRoot) {
		this.webRoot = webRoot;
	}

	@Override
	public void handle(HttpRequest request, HttpResponse response, HttpContext context) throws HttpException, IOException {
		String uri = request.getRequestLine().getUri();
		String target = uri.substring(0, uri.length() - WebServer.SUFFIX_ZIP.length());
		Logdeal.D(TAG,"handle...uri:" + uri + "target:" + target);
		final File file = new File(this.webRoot, target);
		response(response,file,false);
	}

	protected void response(HttpResponse response, final File file, final boolean zip){
		if(file.exists()){
			HttpEntity entity = new EntityTemplate(new ContentProducer() {
				@Override
				public void writeTo(OutputStream outstream) throws IOException {
					if(zip){
						zip(file, outstream);
					}else {
						rawOutput(file,outstream);
					}
				}
			});
			response.setStatusCode(HttpStatus.SC_OK);
			response.setHeader("Content-Type", "application/octet-stream");
			String fileName = file.getName();
			if(!fileName.endsWith(".zip")){
				fileName = fileName + ".zip";
			}
			response.addHeader("Content-Disposition","attachment;filename=" + fileName);
			response.addHeader("Location", file.getPath());
			response.setEntity(entity);
		}else {
			response.setStatusCode(HttpStatus.SC_NOT_FOUND);
			StringEntity entity = null;
			try {
				entity = new StringEntity("<html><body><h1>Error 404, file not found.</h1></body></html>", "UTF-8");
				response.setHeader("Content-Type", "text/html");
				response.setEntity(entity);
			} catch (UnsupportedEncodingException e) {
				e.printStackTrace();
			}
		}
	}

	/**
	 * 压缩目录至输出流
	 * 
	 * @param inputFile 压缩目录
	 * @param outstream 输出流
	 * @throws IOException
	 */
	private void zip(File inputFile, OutputStream outstream) throws IOException {
		ZipOutputStream zos = null;
		try {
			// 创建ZIP文件输出流
			zos = new ZipOutputStream(outstream);
			// 递归压缩文件进zip文件流
			zip(zos, inputFile, inputFile.getName());
		} catch (IOException e) {
			throw e; // 抛出IOException
		}
		try {
			if (null != zos) {
				zos.close();
			}
		} catch (IOException e) {
		}
	}

	/** 递归压缩文件进zip文件流 */
	private void zip(ZipOutputStream zos, File file, String base)
			throws IOException {
		if (file.isDirectory()) { // 目录时
			File[] files = file.listFiles();
			zos.putNextEntry(new ZipEntry(base + "/"));
			base = base.length() == 0 ? "" : base + "/";
			if (null != files && files.length > 0) {
				for (File f : files) {
					zip(zos, f, base + f.getName()); // 递归
				}
			}
		} else {
			zos.putNextEntry(new ZipEntry(base)); // 加入一个新条目
			FileInputStream fis = new FileInputStream(file); // 创建文件输入流
			int count; // 读取计数
			byte[] buffer = new byte[BUFFER_LENGTH]; // 缓冲字节数组
			/* 写入zip输出流 */
			while ((count = fis.read(buffer)) != -1) {
				zos.write(buffer, 0, count);
				zos.flush();
			}
			fis.close(); // 关闭文件输入流
		}
	}

	private void rawOutput(File file, OutputStream os){
		FileInputStream fis = null;
		try {
			fis = new FileInputStream(file); // 创建文件输入流
			int count; // 读取计数
			byte[] buffer = new byte[BUFFER_LENGTH]; // 缓冲字节数组
			/* 写入输出流 */
			while ((count = fis.read(buffer)) != -1) {
                os.write(buffer, 0, count);
                os.flush();
            }
		} catch (IOException e) {
			e.printStackTrace();
		} finally {
			if(fis != null){
				try {
					fis.close(); // 关闭文件输入流
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		}
	}

}
