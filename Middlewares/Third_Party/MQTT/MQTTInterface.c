/**
 ******************************************************************************
 * File Name          : MQTTInterface.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 SmartFactory s.r.l.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 * Contributors:
 *    Federico Parente - initial API and implementation and/or initial documentation
 ******************************************************************************
 */
#include "MQTTInterface.h"
#include "stm32f4xx_hal.h"

#include <string.h>
#include "lwip.h"
#include "lwip/api.h"
#include "lwip/sockets.h"
#include "leds.h"

#ifdef MQTT_LWIP_SOCKET_TLS
#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/debug.h"
#endif

uint32_t MilliTimer;

#ifdef MQTT_LWIP_SOCKET_TLS
mbedtls_net_context server_fd;
const char *pers = "mbedtls";

mbedtls_entropy_context entropy;
mbedtls_ctr_drbg_context ctr_drbg;
mbedtls_ssl_context ssl;
mbedtls_ssl_config conf;
mbedtls_x509_crt cacert;
mbedtls_x509_crt clicert;
mbedtls_pk_context pkey;
#endif
/*old ca 21 righe*/
const char mbedtls_root_certificate[] =
		"-----BEGIN CERTIFICATE-----\r\n"
		"MIIFAzCCAuugAwIBAgIUNsv6chQfxRfjyuN2RDOsnho9+oowDQYJKoZIhvcNAQEL\r\n"
		"BQAwETEPMA0GA1UEAwwGQ0FfTUNVMB4XDTI2MDIwMjEwMjEwNVoXDTM2MDEzMTEw\r\n"
		"MjEwNVowETEPMA0GA1UEAwwGQ0FfTUNVMIICIjANBgkqhkiG9w0BAQEFAAOCAg8A\r\n"
		"MIICCgKCAgEAu31/PkNM8c5lOXIrk+IsPbGTeqD2vJY/Pk5GOtlYQxuOlEM/yO5v\r\n"
		"LEhYUBCYYtD0/p6VQdmt3xbgJ9tpsYiFpaZLtRhFQJqd83HVnGmIeuYhtAbcqIUZ\r\n"
		"cQQHARO/nyO4B1Jt+kIvUrNIXGG1dLmQoYfZPrsDTB7mO/5y2RKg85KY4JN+duuU\r\n"
		"k0GLTFSiVGEKSTGfkislJKKToTib0jeSf4cCNs6A+Cxour2+ov775LGRPV7gpVTP\r\n"
		"OqNUaS4TRmqCYUUbbQg4rB1GfvhBJDsnyAHnW7KV5531G5XBopQ3WqLikP/nwGJV\r\n"
		"Cic4uHPg+yVRMcEC7phgeV64VE5UoWyMxJymzqAwBW6O1qhKclTJUi141N/ohHze\r\n"
		"VWFrJm3std8lGOhjm/6IcD4wcAxS5hLCuG1tuYlV/5AOTusQkh2ilXw1fHxTp48L\r\n"
		"5ZtyrsKK/pnKVzyLXKcaMdfSDC1TL1SH78RhDeHZKBM7nmym8dAxqv7foBCaJ+3d\r\n"
		"Dt7R9Sx/jItu6Nma1l0/TDUhryBJ4Ar2/9uN6aCsLh45hV61RTwHbkswj1UIlfGm\r\n"
		"om4B3YZhSDUQlZstL/3RbweitwPQ1shZkdx5i1cREUMSj5AKGKJar2Epy+wz8v95\r\n"
		"bTuMLqtj/5GtRZCyZ/b0rfWvEdvqEsLgftDmAZ8+07uQ6mT2EiMZ1MECAwEAAaNT\r\n"
		"MFEwHQYDVR0OBBYEFC0a+LGqx929hLiYDNO5/PZsGXrMMB8GA1UdIwQYMBaAFC0a\r\n"
		"+LGqx929hLiYDNO5/PZsGXrMMA8GA1UdEwEB/wQFMAMBAf8wDQYJKoZIhvcNAQEL\r\n"
		"BQADggIBAEEo3Uu7mbQ/dkNXYRVwwvC/1l77s6FbAcqhmV7xuZtBJi9qEqnAHOZd\r\n"
		"kQeMq2jpF00uOF0SoRL6Zay13OQDN0bhnM0f2wc0Ji/xNxLPBOILvTIjB5TyUVCn\r\n"
		"RUfZFq8mgrHOEJOK3Pd2wWSDQJwji6k161+oNaHQ2DNY/aETAKWSRgRAHSul3gmy\r\n"
		"3LzUriuredFZwsqKCMelwzWMaYN74CUjvTAZV0WS1MwVNnN2gR46+Cb+DQeGjL33\r\n"
		"sZgkLZp0jKcefIDB9yH2FJ6KHxHPOnRWzgevY1VG2aY7t0xKo46PyvFa6w/mGZoE\r\n"
		"MLuOgWUsECgIuGfVzd5GJj+meUu6Jk5nK4SkrNlKmIKa5p6bnVRVPN3DgtT2sVib\r\n"
		"2MP/3L5x81ccNPqYio1Napu2MjcwcRu32l2wOEPRPy4Hz2ecSZ6NSWCO3uFU0rX4\r\n"
		"5TVbpMJFxlr5ho8Gsydd/5XGMVJtzisNSsr43FfuKi/5xR711qGqBG/ai+jp4Z6g\r\n"
		"70XV3b4mFRBxbKNwlBqj9/F5vY3m9WNjjPJcAHyg+anKC24GIuVgQux51/HILVfU\r\n"
		"o0YAiiWlZpYxr8Hi7c13IqQJavpv8ewH/G9ez9beN9yelcafExlgRcufELCtqA3p\r\n"
		"frScrOMdIHhM7R9SyoiKLIwJ2dWS6SR6ZGSHWnk+rOqEftStFAW9\r\n"
		"-----END CERTIFICATE-----\r\n";

const size_t mbedtls_root_certificate_len = sizeof(mbedtls_root_certificate);

/*old cert 22 righe*/
const char client_cert[] =
		"-----BEGIN CERTIFICATE-----\r\n"
		"MIID8TCCAdmgAwIBAgIUBndDyZ/pJxzDUIwPoxuh3EHTeXEwDQYJKoZIhvcNAQEL\r\n"
		"BQAwETEPMA0GA1UEAwwGQ0FfTUNVMB4XDTI2MDIwMjEwMjExM1oXDTI3MDIwMjEw\r\n"
		"MjExM1owEDEOMAwGA1UEAwwFbWN1MDEwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAw\r\n"
		"ggEKAoIBAQCk9qP30qpYie7EwUeHwTh0htaO/SpP9fmvUheOzwWt3n8PQV0JFyw0\r\n"
		"SGLPeTE4ECr/+AH7Yw2z274BUfFxch5ysbi/qAM5wnI3K8hfv/1VeJ+ojmaxC93o\r\n"
		"60CWejeN3v8cXVeczz0Go7g7I9ccWN53/rpxjrAohDf5LS1CJyaTyFJW1vn4lDPR\r\n"
		"kzbsZSwdOfYJIvufAnkPrTjmM9Ny10SOF5zyG1VBZ5wWHHxQMZgKFXfvOAcg4444\r\n"
		"ADJl6C/wBEPyLk0+DbDLvTVAyuuGtQbSEMB46Hk/pMNUutfoQ+iJ49lmRbVm3+Q2\r\n"
		"GbZrCzDQ7kcECTJ3EDdINhsE8AgWN0OXAgMBAAGjQjBAMB0GA1UdDgQWBBRSEJ7D\r\n"
		"ri8Tyoll3FPIuBE/iaKjtzAfBgNVHSMEGDAWgBQtGvixqsfdvYS4mAzTufz2bBl6\r\n"
		"zDANBgkqhkiG9w0BAQsFAAOCAgEAA8wQCptm7bQovUarfNjaenUuRiuQ0xlyCXRL\r\n"
		"lnmMWGrSvTnTLKQPO01THYdetW2oX3Ydn5grkc4XsXE5Wn6lAreDY6L7djY1iVUc\r\n"
		"zsX2tzMGxSmzz6WaTC9GOrmIn1SA89rWkFZF/FA/KMzp9MJJZZLPuxBUghZnxmuq\r\n"
		"6SY5F97kW4veX1yfCxrPDi/KaC62dok8khWuHmC7tdJOQq0FsJL9TQQKB4EWvDf2\r\n"
		"Y8HhCYusjXfu8uFhcV/AYqRlhFy6/i20pZ4Sd46IhV315dSxW3BUW8bkRdKzdWLJ\r\n"
		"KIFHuwCTqNi0eQF7pVhiZxF2thz7cKRFih9JbISPg0mSG7Tynn058eneiG6K5s1R\r\n"
		"cb7gdpo5YYnATB2KDAXkvVhCWvTccSzIQbY493JOhOfG0R1TV2/YScmQjTS+1N5t\r\n"
		"urR8i/6LcaFppAKYleDLO9SCITenSNDWu6O8i2+T2ogYJixNi8+NyIjIYb3CWny/\r\n"
		"vjAbqMv98unN4WUG/7zBeUPTGNvZ5XgKQlvSObvqwoYeMR9Ks7zvYWSSZyEZNrZz\r\n"
		"8PR8ZddgCmSJ/pQUBN0e2s8Ku/u22/PXZZRmOUAsu3pu/iO10Jy8FfNg2Jy2QUf6\r\n"
		"yJhg+tcQfFmKx69/aGsHuNiYhatCvE8A1ALVWF+h1LBxmTbNDAcX0xXxdI/13ydn\r\n"
		"/ztrswI=\r\n"
		"-----END CERTIFICATE-----\r\n";

const size_t client_cert_len = sizeof(client_cert);

/*old key 27 righe*/
const char client_key[] =
		"-----BEGIN PRIVATE KEY-----\r\n"
		"MIIEvwIBADANBgkqhkiG9w0BAQEFAASCBKkwggSlAgEAAoIBAQCk9qP30qpYie7E\r\n"
		"wUeHwTh0htaO/SpP9fmvUheOzwWt3n8PQV0JFyw0SGLPeTE4ECr/+AH7Yw2z274B\r\n"
		"UfFxch5ysbi/qAM5wnI3K8hfv/1VeJ+ojmaxC93o60CWejeN3v8cXVeczz0Go7g7\r\n"
		"I9ccWN53/rpxjrAohDf5LS1CJyaTyFJW1vn4lDPRkzbsZSwdOfYJIvufAnkPrTjm\r\n"
		"M9Ny10SOF5zyG1VBZ5wWHHxQMZgKFXfvOAcg4444ADJl6C/wBEPyLk0+DbDLvTVA\r\n"
		"yuuGtQbSEMB46Hk/pMNUutfoQ+iJ49lmRbVm3+Q2GbZrCzDQ7kcECTJ3EDdINhsE\r\n"
		"8AgWN0OXAgMBAAECggEAD7tdw8KZcDwnQyhTlAS/R42cZKKMKHgCyc+AY6QmnIA9\r\n"
		"iczn/HfzRUEp+zNJmFkiYKrcNYcqxV5Nbn7MEc+aS8HbJpv1QsribZSFVWxP0pjg\r\n"
		"UQjTjkrdbNsSm8coCSWdGq1kL5Yr8bgnV6yBVTi8bqEiUrYS4qpJVEAUstxc93bZ\r\n"
		"xvJg5nGLrXj1CmOMpwFh48ZWq/UCvqgj6/DSsh0sMv1QgxZMb/P7T1iH6ipGsBqi\r\n"
		"fGKBOjcrvtcqzjWht2Ql2tENifl5EU0tsUYTKq1/N7SIXVEfMQDWhglMfxOD7n1C\r\n"
		"FEFdoi6YGRuO6Ph+XKujZJgWX+hztzv/AzHYoA/FEQKBgQDk3Fn4rJmhD+jvUwNf\r\n"
		"0cfupc7Tb7QueRIEiFvSmUoYhMA2UfkxRaP9QGOIfkL/XnwdsslnCf2BQQTTeJJX\r\n"
		"aBfxjPz24hacCaSh5EFXBsn2y00ir+r0EgcrVJrlF6Y2v36A7pdgKtqDw5L+ohVh\r\n"
		"fdUvLQFuBgFJ0guin2/R8ELoJQKBgQC4hoYvtjX11odcWgigXOSZKIA2LlX2jVAk\r\n"
		"Y4wzhRjqKon3R6BmFSRl7ZFVW7axJhjgtPpQmPil6LDBlZTDb4Q7gAZ6hpqh9OBW\r\n"
		"efTLmdwTVf2x3JcZcsE/kq/cMIs4oXgU/EASu+OIRc/fKrcWI0G5DIueblIzBkZl\r\n"
		"GLTkuTsCCwKBgQCcnAlZeUs8AzYkJ4tkk5qcPYyV9Ze4QZNJ1MH5EHq64oTh5yB+\r\n"
		"TiiVEe3KppRUSiTm8cSOpTB7tbGGwLhr5ciUvCUTEM3mA1dMjXUFB5lHMAHd3cIL\r\n"
		"ORb82M2SPae5STtZIWwnG8wiZL//TY0SBNjQipyy3JsfGARsSWxsnlw6nQKBgQCX\r\n"
		"4mLhMuIHkKWqZpFjLhMb3jJqZolrf7XPvgDRB2USmiEl4Vctr4lePMW7tNoTxoAD\r\n"
		"tfO0E7XXGkkjjrP8ifYV47Nxdwog9ScTBx9v1x1G7Sj9Fc1prQNUCLGoVGzEGGR3\r\n"
		"IYq74NvQd2Y7EzXQiwS2BJWHutb+GNQossVy3uSrMQKBgQDKt0QfX8nSdc1o7c8D\r\n"
		"TXRmPjfiw5oeGMsVqU4W9VrkKZF4/w7XtD7EH5BiLNqAWbfnjc0s+E+pyX6Kc4rD\r\n"
		"29sFzwzRIK2DV7BiTOWc+aNBUzCVAu1eZqffwfsyCtuYeWuDRSa+3XewFB69AVFW\r\n"
		"Z49A8UNRjfcoTIArbVN9u5a4gw==\r\n"
		"-----END PRIVATE KEY-----\r\n";

const size_t client_key_len = sizeof(client_key);


#ifdef MQTT_LWIP_SOCKET
void mqtt_network_init(Network *n) {
	n->socket = 0; //clear
	n->mqttread = mqtt_network_read; //receive function
	n->mqttwrite = mqtt_network_write; //send function
	n->disconnect = mqtt_network_disconnect; //disconnection function
}

int mqtt_network_connect(Network *n, char *ip, int port) {
	struct sockaddr_in server_addr;

	if(n->socket)
	{
		close(n->socket);
	}

	n->socket = socket(PF_INET, SOCK_STREAM, 0); //create socket
	if(n->socket < 0)
	{
		n->socket = 0;
		return -1;
	}

	memset(&server_addr, 0, sizeof(struct sockaddr_in)); //broker address info
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip);
	server_addr.sin_port = htons(port);

	if(connect(n->socket, (struct sockaddr*)&server_addr, sizeof(struct sockaddr_in)) < 0) //connect to the broker
	{
		close(n->socket);
		return -1;
	}
	return 0;
}

int mqtt_network_read(Network *n, unsigned char *buffer, int len, int timeout_ms) {
	int available;

	/* !!! LWIP_SO_RCVBUF must be enabled !!! */
	if(ioctl(n->socket, FIONREAD, &available) < 0) return -1; //check receive buffer

	if(available > 0)
	{
		return recv(n->socket, buffer, len, 0);
	}

	return 0;
}

int mqtt_network_write(Network *n, unsigned char *buffer, int len, int timeout_ms) {
	return send(n->socket, buffer, len, 0);
}

void mqtt_network_disconnect(Network *n) {
	close(n->socket);
	n->socket = 0;
}
#endif
#ifdef MQTT_LWIP_SOCKET_TLS

static void my_debug(void *ctx, int level, const char *file, int line, const char *str) {
	((void) level);
	//mbedtls_fprintf((FILE*) ctx, "%s:%04d: %s", file, line, str);
	//fprintf((FILE*) ctx, "%s:%04d: %s", file, line, str);
	MQTT_INTERFACE_DEBUG_LOG("[MQTT_INTERFACE]: %s:%04d: %s", file, line, str);
	fflush((FILE*) ctx);
}


void mqtt_network_init(Network *n) {
	n->socket = 0; //clear
	n->mqttread = mqtt_network_read; //receive function
	n->mqttwrite = mqtt_network_write; //send function
	n->disconnect = mqtt_network_disconnect; //disconnection function
}

int mqtt_network_connect(Network *n, char *ip, char * port) {
	int ret = 0;

#if defined(MBEDTLS_DEBUG_C) && defined(DEBUG)
	mbedtls_debug_set_threshold(99);
#endif

	// Initialize the network interface
	mqtt_network_init(n);
	mqtt_network_clear();

	//mbedtls_net_init( &server_fd ); // MX_LWIP_Init() is called already
	mbedtls_ssl_init(&ssl);
	mbedtls_ssl_config_init(&conf);
	mbedtls_x509_crt_init(&cacert);
	mbedtls_x509_crt_init(&clicert);
	mbedtls_pk_init(&pkey);
	mbedtls_ctr_drbg_init(&ctr_drbg);
	mbedtls_entropy_init(&entropy);

	ret = psa_crypto_init();
	if(ret != PSA_SUCCESS) {
		MQTT_INTERFACE_DEBUG_LOG("[MQTT_INTERFACE] ERROR: psa_crypto_init failed.\n");
		return -1;
	}

	if( ( ret = mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy,
	                           (const unsigned char *) pers,
	                           strlen( pers ) ) ) != 0 )
	{
		MQTT_INTERFACE_DEBUG_LOG("[MQTT_INTERFACE] ERROR: mbedtls_ctr_drbg_seed returned %d\n", ret );
	    return -1;
	}

	// Processi SSL/TLS
	ret = mbedtls_x509_crt_parse(&cacert, (const unsigned char*) mbedtls_root_certificate, mbedtls_root_certificate_len);
	if (ret < 0) {
		MQTT_INTERFACE_DEBUG_LOG("[MQTT_INTERFACE] ERROR: mbedtls_x509_crt_parse failed.\n");
	    return -1;
	}

	// START
	// TLS V1.3
#if !defined(TLS_1V2) && defined(TLS_1V3)
	ret = mbedtls_x509_crt_parse(&clicert, (const unsigned char *)client_cert, client_cert_len);
	if (ret != 0) {
		MQTT_INTERFACE_DEBUG_LOG("[MQTT_INTERFACE] ERROR: mbedtls_x509_crt_parse failed\n");
	    return -1;
	}

	// Aggiungi caricamento della chiave cliente
	ret = mbedtls_pk_parse_key(&pkey, (const unsigned char *) client_key, client_key_len, NULL, 0, mbedtls_ctr_drbg_random, &ctr_drbg);
	if (ret != 0) {
		MQTT_INTERFACE_DEBUG_LOG("[MQTT_INTERFACE] ERROR: mbedtls_pk_parse_key failed.\n");
	    return -1;
	}

	// Configura il certificato e la chiave privata nel contesto SSL
	ret = mbedtls_ssl_conf_own_cert(&conf, &clicert, &pkey);
	if (ret != 0) {
		MQTT_INTERFACE_DEBUG_LOG("[MQTT_INTERFACE] ERROR: mbedtls_ssl_conf_own_cert failed.\n");
	    return -1;
	}
#endif
	// END

	ret = mbedtls_ssl_config_defaults(&conf, MBEDTLS_SSL_IS_CLIENT,
	        MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
	if (ret < 0) {
		MQTT_INTERFACE_DEBUG_LOG("[MQTT_INTERFACE] ERROR: mbedtls_ssl_config_defaults failed.\n");
	    return -1;
	}

	mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_REQUIRED);
	mbedtls_ssl_conf_ca_chain(&conf, &cacert, NULL);
	mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
	mbedtls_ssl_conf_dbg(&conf, my_debug, stdout);

	// TLS V1.2
#if defined(TLS_1V2) && !defined(TLS_1V3)
	mbedtls_ssl_conf_min_version(&conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_3);
	mbedtls_ssl_conf_max_version(&conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_3);
#endif
	// TLS V1.3
#if !defined(TLS_1V2) && defined(TLS_1V3)
	mbedtls_ssl_conf_min_version(&conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_4);
	mbedtls_ssl_conf_max_version(&conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_4);
#endif

	ret = mbedtls_ssl_setup(&ssl, &conf);
	if (ret < 0) {
		MQTT_INTERFACE_DEBUG_LOG("[MQTT_INTERFACE] ERROR: mbedtls_ssl_setup failed.\n");
	    return -1;
	}

	ret = mbedtls_ssl_set_hostname(&ssl, ip); // if the handshake fail check here
	if (ret < 0) {
		MQTT_INTERFACE_DEBUG_LOG("[MQTT_INTERFACE] ERROR: mbedtls_ssl_set_hostname failed.\n");
	    return -1;
	}

	mbedtls_ssl_set_bio(&ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);

	// register functions
	n->mqttread = mqtt_network_read; //receive function
	n->mqttwrite = mqtt_network_write; //send function
	n->disconnect = mqtt_network_disconnect; //disconnection function



	// Connect

	ret = mbedtls_net_connect(&server_fd, (const char*)ip, port, MBEDTLS_NET_PROTO_TCP);
	if (ret < 0) {
		MQTT_INTERFACE_DEBUG_LOG("[MQTT_INTERFACE] ERROR: mbedtls_net_connect failed.\n");
		return -1;
	}

	while ((ret = mbedtls_ssl_handshake(&ssl)) != 0) {
		if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
			MQTT_INTERFACE_DEBUG_LOG("[MQTT_INTERFACE] ERROR: mbedtls_ssl_handshake failed.\n");
			return -2;
		}
	}

	ret = mbedtls_ssl_get_verify_result(&ssl);
	if (ret < 0) {
		MQTT_INTERFACE_DEBUG_LOG("[MQTT_INTERFACE] ERROR: mbedtls_ssl_get_verify_result failed.\n");
		return -1;
	}

	return 0;
}


int mqtt_network_read(Network *n, unsigned char *buffer, int len, int timeout_ms) {
	int ret;
	int received = 0;
	int error = 0;
	int complete = 0;

	//set timeout
	if (timeout_ms != 0) {
		mbedtls_ssl_conf_read_timeout(&conf, timeout_ms);
	}

	//read until received length is bigger than variable len
	do {
		ret = mbedtls_ssl_read(&ssl, buffer, len);
		if (ret > 0) {
			received += ret;
		} else if (ret != MBEDTLS_ERR_SSL_WANT_READ) {
			error = 1;
		}
		if (received >= len) {
			complete = 1;
		}
	} while (!error && !complete);

	return received;
}


int mqtt_network_write(Network *n, unsigned char *buffer, int len, int timeout_ms) {
	int ret;
	int written;

	//check all bytes are written
	for (written = 0; written < len; written += ret) {
		while ((ret = mbedtls_ssl_write(&ssl, buffer + written, len - written)) <= 0) {
			if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
				return ret;
			}
		}
	}

	return written;
}


void mqtt_network_disconnect(Network *n) {
	int ret;

	do {
		ret = mbedtls_ssl_close_notify(&ssl);
	} while (ret == MBEDTLS_ERR_SSL_WANT_WRITE);

	mbedtls_ssl_session_reset(&ssl);
	mbedtls_net_free(&server_fd);
}


void mqtt_network_clear() {
	mbedtls_net_free(&server_fd);
	mbedtls_x509_crt_free(&cacert);
	mbedtls_x509_crt_free(&clicert);
	mbedtls_pk_free(&pkey);
	mbedtls_psa_crypto_free();
	mbedtls_ssl_free(&ssl);
	mbedtls_ssl_config_free(&conf);
	mbedtls_ctr_drbg_free(&ctr_drbg);
	mbedtls_entropy_free(&entropy);
}

#endif
#ifdef MQTT_LWIP_NETCONN
void mqtt_network_init(Network *n) {
	n->conn = NULL;
	n->buf = NULL;
	n->offset = 0;

	n->mqttread = mqtt_network_read;
	n->mqttwrite = mqtt_network_write;
	n->disconnect = mqtt_network_disconnect;
}

int mqtt_network_connect(Network *n, char *ip, int port) {
	err_t err;
	ip_addr_t server_ip;

	ipaddr_aton(ip, &server_ip);

	n->conn = netconn_new(NETCONN_TCP);
	if (n->conn != NULL) {
		err = netconn_connect(n->conn, &server_ip, port);

		if (err != ERR_OK) {
			netconn_delete(n->conn); //free memory
			return -1;
		}
	}

	return 0;
}

int mqtt_network_read(Network *n, unsigned char *buffer, int len, int timeout_ms) {
	int rc;
	struct netbuf *inbuf;
	int offset = 0;
	int bytes = 0;

	while(bytes < len) {
		if(n->buf != NULL) {
			inbuf = n->buf;
			offset = n->offset;
			rc = ERR_OK;
		} else {
			rc = netconn_recv(n->conn, &inbuf);
			offset = 0;
		}

		if(rc != ERR_OK) {
			if(rc != ERR_TIMEOUT) {
				bytes = -1;
			}
			break;
		} else {
			int nblen = netbuf_len(inbuf) - offset;
			if((bytes+nblen) > len) {
				netbuf_copy_partial(inbuf, buffer+bytes, len-bytes,offset);
				n->buf = inbuf;
				n->offset = offset + len - bytes;
				bytes = len;
			} else {
				netbuf_copy_partial(inbuf, buffer+bytes, nblen, offset);
				bytes += nblen;
				netbuf_delete(inbuf);
				n->buf = NULL;
				n->offset = 0;
			}
		}
	}
	return bytes;
}

int mqtt_network_write(Network *n, unsigned char *buffer, int len, int timeout_ms) {
	int rc = netconn_write(n->conn, buffer, len, NETCONN_NOCOPY);
	if(rc != ERR_OK) return -1;
	return len;
}

void mqtt_network_disconnect(Network *n) {
	netconn_close(n->conn); //close session
	netconn_delete(n->conn); //free memory
	n->conn = NULL;
}
#endif

#ifdef MQTT_TASK
int ThreadStart(Thread* thread, void (*fn)(void*), void* arg)
{
	int rc = 0;
	uint16_t usTaskStackSize = (configMINIMAL_STACK_SIZE * 5);
	UBaseType_t uxTaskPriority = uxTaskPriorityGet(NULL); /* set the priority as the same as the calling task*/

	rc = xTaskCreate(fn,	/* The function that implements the task. */
		"MQTTTask",			/* Just a text name for the task to aid debugging. */
		usTaskStackSize,	/* The stack size is defined in FreeRTOSIPConfig.h. */
		arg,				/* The task parameter, not used in this case. */
		uxTaskPriority,		/* The priority assigned to the task is defined in FreeRTOSConfig.h. */
		&thread->task);		/* The task handle is not used. */

	return rc;
}


void MutexInit(Mutex* mutex)
{
	mutex->sem = xSemaphoreCreateMutex();
}

int MutexLock(Mutex* mutex)
{
	return xSemaphoreTake(mutex->sem, portMAX_DELAY);
}

int MutexUnlock(Mutex* mutex)
{
	return xSemaphoreGive(mutex->sem);
}
#endif

//Timer functions
char TimerIsExpired(Timer *timer) {
	long left = timer->end_time - MilliTimer;
	return (left < 0);
}

void TimerCountdownMS(Timer *timer, unsigned int timeout) {
	timer->end_time = MilliTimer + timeout;
}

void TimerCountdown(Timer *timer, unsigned int timeout) {
	timer->end_time = MilliTimer + (timeout * 1000);
}

int TimerLeftMS(Timer *timer) {
	long left = timer->end_time - MilliTimer;
	return (left < 0) ? 0 : left;
}

void TimerInit(Timer *timer) {
	timer->end_time = 0;
}

