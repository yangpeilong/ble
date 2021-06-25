package com.qdingnet.provider.config.upgrade;

import com.qdingnet.provider.config.Logdeal;

import org.apache.http.HttpException;
import org.apache.http.HttpRequest;
import org.apache.http.HttpResponse;
import org.apache.http.protocol.HttpContext;

import java.io.File;
import java.io.IOException;

/**
 * Created by pipan on 17-8-30.
 */

public class HttpHackQdZipHandler extends HttpZipHandler {
    private final static String TAG = "HttpHackQdZipHandler";

    public HttpHackQdZipHandler(String webRoot) {
        super(webRoot);
    }

    @Override
    public void handle(HttpRequest request, HttpResponse response, HttpContext context) throws HttpException, IOException {
        String target = request.getRequestLine().getUri();
        int endIndex = target.indexOf("?");
        if(endIndex > 0){
            target = target.substring(0, endIndex);
        }
        Logdeal.D(TAG,"handle...target:" + target);
        final File file = new File(this.webRoot, target);
        response(response,file, false);
    }
}
