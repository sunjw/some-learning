package org.sunjw.util;

import okhttp3.Authenticator;
import okhttp3.Credentials;
import okhttp3.JavaNetCookieJar;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.Response;
import okhttp3.Route;

import javax.net.ssl.*;
import java.io.IOException;
import java.net.CookieManager;
import java.net.CookiePolicy;
import java.net.InetSocketAddress;
import java.net.Proxy;
import java.security.KeyManagementException;
import java.security.NoSuchAlgorithmException;
import java.security.cert.CertificateException;
import java.util.concurrent.TimeUnit;

public class HttpUtil {

    private static void prepareHttpClientBuilder(OkHttpClient.Builder builder) {
        CookieManager cookieManager = new CookieManager();
        cookieManager.setCookiePolicy(CookiePolicy.ACCEPT_ALL);
        builder.cookieJar(new JavaNetCookieJar(cookieManager));
        builder.connectTimeout(15, TimeUnit.SECONDS); // 15s
        builder.writeTimeout(15, TimeUnit.SECONDS); // 15s
        builder.readTimeout(30, TimeUnit.SECONDS); // 30s
    }

    private static void prepareHttpClientBuilderProxy(OkHttpClient.Builder builder,
                                                      String server,
                                                      int port,
                                                      String username,
                                                      String password) {
        Proxy proxy = new Proxy(Proxy.Type.HTTP, new InetSocketAddress(server, port));
        builder.proxy(proxy);
        if (!CommUtil.isStringNullOrEmpty(username)) {
            Authenticator proxyAuthenticator = new Authenticator() {
                @Override
                public Request authenticate(Route route, Response response) throws IOException {
                    String credential = Credentials.basic(username, password);
                    return response.request().newBuilder()
                            .header("Proxy-Authorization", credential)
                            .build();
                }
            };
            builder.proxyAuthenticator(proxyAuthenticator);
        }
    }

    public static OkHttpClient createHttpClient() {
        OkHttpClient.Builder builder = new OkHttpClient.Builder();
        prepareHttpClientBuilder(builder);
        return builder.build();
    }

    public static OkHttpClient createHttpClientWithProxy(String server,
                                                         int port,
                                                         String username,
                                                         String password) {
        OkHttpClient.Builder builder = new OkHttpClient.Builder();
        prepareHttpClientBuilder(builder);
        prepareHttpClientBuilderProxy(builder, server, port, username, password);
        return builder.build();
    }

    public static OkHttpClient createHttpClientIgnoreCert() throws NoSuchAlgorithmException, KeyManagementException {
        // Create a trust manager that does not validate certificate chains
        final TrustManager[] trustAllCerts = new TrustManager[]{
                new X509TrustManager() {
                    @Override
                    public void checkClientTrusted(java.security.cert.X509Certificate[] chain, String authType) throws CertificateException {
                    }

                    @Override
                    public void checkServerTrusted(java.security.cert.X509Certificate[] chain, String authType) throws CertificateException {
                    }

                    @Override
                    public java.security.cert.X509Certificate[] getAcceptedIssuers() {
                        return new java.security.cert.X509Certificate[]{};
                    }
                }
        };

        // Install the all-trusting trust manager
        final SSLContext sslContext = SSLContext.getInstance("SSL");
        sslContext.init(null, trustAllCerts, new java.security.SecureRandom());
        // Create an ssl socket factory with our all-trusting manager
        final SSLSocketFactory sslSocketFactory = sslContext.getSocketFactory();

        OkHttpClient.Builder builder = new OkHttpClient.Builder();
        prepareHttpClientBuilder(builder);
        builder.sslSocketFactory(sslSocketFactory, (X509TrustManager) trustAllCerts[0]);
        builder.hostnameVerifier(new HostnameVerifier() {
            @Override
            public boolean verify(String hostname, SSLSession session) {
                return true;
            }
        });

        return builder.build();
    }

}
