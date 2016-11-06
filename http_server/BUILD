cc_library(
    name = 'http_server',
    srcs = [
        'evhtp-executor-service.cpp',
        'evhtp-http-request.cpp',
        'evhtp-http-response.cpp',
        'evhtp-http-server.cpp',
        'handler-router.cpp',
        'http-handler.cpp',
        'http-response.cpp',
        'http-server.cpp',
    ],
    deps = [
        '//common/http_server/evhtp:evhtp',
        '//common/base:base',
        '//thirdparty/boost:boost',
    ]
)


cc_binary(
    name = 'http-server_example',
    srcs = ['http-server_example.cpp'],
    deps = [':http_server']
)

