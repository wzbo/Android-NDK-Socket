#include <string.h>
#include <jni.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include "unit.h"
#include <android/log.h>


#define TAG "tcomm"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

#define MAXLINE 4096

static int sockfd;


/**
 * 创建SOCKET
 */
jboolean
Java_com_wuzongbo_sample_MainActivity_createSocket(JNIEnv *env,
                                                             jclass clazz) {
    LOGV("createSocket...");
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        LOGE("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        return JNI_FALSE;
    }
    return JNI_TRUE;
}


/**
 * 连接SOCKET
 */
jint
Java_com_wuzongbo_sample_MainActivity_connectSocket(JNIEnv *env,
                                                              jclass clazz, jstring ip, jint port) {
    LOGV("connectSocket...");
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    char *ipaddr = jstringTostring(env, ip);
    if (inet_pton(AF_INET, ipaddr, &servaddr.sin_addr) <= 0) {
        LOGE("inet_pton error for %s\n", ip);
        return JNI_FALSE;
    }
    LOGV("connect...");
    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        LOGE("connect error: %s(errno: %d)\n", strerror(errno), errno);
        return JNI_FALSE;
    }
    LOGV("connect succeed!!!");
    return JNI_TRUE;
}

/**
 * 接收数据
 */
jint
Java_com_wuzongbo_sample_MainActivity_recvSocket(JNIEnv *env,
                                                           jclass clazz, jbyteArray buffer,
                                                           jint offset,
                                                           jint count) {
    LOGV("recvSocket");
    if (!sockfd) {
        LOGE("socket is failed!");
        return 0;
    }
    jbyte buf[count - offset];
    int rec_len;
    if ((rec_len = (int) recv(sockfd, buf, sizeof(buf), 0)) == -1) {
        LOGE("recv error");
        return 0;
    }

    buf[rec_len] = '\0';
    (*env)->SetByteArrayRegion(env, buffer, offset, count, buf);
    return rec_len;
}

/**
 * 发送数据
 */
jboolean
Java_com_wuzongbo_sample_MainActivity_sendSocket(JNIEnv *env,
                                                           jclass clazz, jstring sendData) {
    LOGV("sendSocket");
    if (!sockfd) {
        printf("socket is failed! \n");
        return JNI_FALSE;
    }
    char *data = jstringTostring(env, sendData);
    fgets(data, 4096, stdin);
    if (send(sockfd, data, strlen(data), 0) < 0) {
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

/**
 * 关闭socket
 */
jboolean
Java_com_wuzongbo_sample_MainActivity_closeSocket(JNIEnv *env,
                                                            jclass clazz) {
    LOGV("closeSocket");
    if (!sockfd)
        return JNI_FALSE;
    close(sockfd);
    return JNI_TRUE;
}


