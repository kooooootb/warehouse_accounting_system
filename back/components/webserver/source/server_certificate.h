#ifndef H_2641FB6B_BE30_4EF1_87E8_8C58EE4B718C
#define H_2641FB6B_BE30_4EF1_87E8_8C58EE4B718C

#include <cstddef>
#include <memory>

#include <boost/asio/buffer.hpp>
#include <boost/asio/ssl/context.hpp>

namespace ws
{

/*  Load a signed certificate into the ssl context, and configure
    the context for use with a server.
    TODO: hide key in files
*/
inline void LoadServerCertificate(boost::asio::ssl::context& ctx)
{
    /*
        openssl dhparam -out dh.pem 2048
        openssl req -newkey rsa:2048 -nodes -keyout key.pem -x509 -days 10000 -out cert.pem
    */

    std::string const cert =
        "-----BEGIN CERTIFICATE-----\n"
        "MIIDuTCCAqGgAwIBAgIUGI5tVc3LquTjSYletagtXHTkh64wDQYJKoZIhvcNAQEL\n"
        "BQAwazELMAkGA1UEBhMCUlUxDzANBgNVBAgMBk1vc2NvdzEPMA0GA1UEBwwGTW9z\n"
        "Y293MSEwHwYDVQQKDBhJbnRlcm5ldCBXaWRnaXRzIFB0eSBMdGQxFzAVBgNVBAMM\n"
        "Djk1LjE0MC4xNDcuMTcwMCAXDTI0MDUxMjEwMDY1OVoYDzIwNTEwOTI4MTAwNjU5\n"
        "WjBrMQswCQYDVQQGEwJSVTEPMA0GA1UECAwGTW9zY293MQ8wDQYDVQQHDAZNb3Nj\n"
        "b3cxITAfBgNVBAoMGEludGVybmV0IFdpZGdpdHMgUHR5IEx0ZDEXMBUGA1UEAwwO\n"
        "OTUuMTQwLjE0Ny4xNzAwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDA\n"
        "OXlbhQSEgRSbNwp1qYXeMrfm9mVoyPEt0MkuU0r5KS0YtOKmrQ6b3kVzMvZgQLKP\n"
        "ulTkGNDhymlHJbb7wpJC42cZWkRJ/EkQdO67pxE9icgA3pZcPC36Wk7E9LnjEqyM\n"
        "oL4EWvouXcDiuDwb3u2gPmjtqYoQuUFopxdpDk9wwMz6oR85ncJjIzJfD+c+33IR\n"
        "pcWSwrNcHLeD4t/HQeH8PLGOzjyQUgyYRcaSeTjBjUQvN68emUtg0WCC3nQ1WINm\n"
        "+O7ASeX53le2ba4WyNiBUwm/Bc/rl6uxPNj6bcJWd3hMeAF8c0NreoHpRKdgj79r\n"
        "u1LcFFqDGN0kb8dMwAQ/AgMBAAGjUzBRMB0GA1UdDgQWBBRhhW4lUpd5Nxchf2mX\n"
        "3yuawwtYBTAfBgNVHSMEGDAWgBRhhW4lUpd5Nxchf2mX3yuawwtYBTAPBgNVHRMB\n"
        "Af8EBTADAQH/MA0GCSqGSIb3DQEBCwUAA4IBAQCVLSvQnM/6Tgok0eJjaaK5Q6VK\n"
        "+aOxXH42ZkkV+OZWdXtysEkVmLOb2RtQgLjUzQja9XDCqlgm2DulzuEYTR4W72p0\n"
        "ed1C0cSo2cK90Olc2Bag/b5Fs1q4/rCuPbS/HjNneVJalCtGD1DwB2Gnsa/DfCnL\n"
        "txLYszaU8KJD6CW/wcFL5K+KJO1HFmsogLw7GX6M7FN4qGSXwOxZzkS4ycwloqSP\n"
        "sgJUC8oXJoK86Uadbc5qkY89twmSkyN5UNHT8Dr3VlvOb3hWkPG9AYjkmKHiGOEn\n"
        "e0bpqz18ywDGtIpJAAHn8Dc+gmyC1QAwcNNtPi/6iRdkiXHQcYQBpWuINq6y\n"
        "-----END CERTIFICATE-----\n";

    std::string const key =
        "-----BEGIN PRIVATE KEY-----\n"
        "MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQDAOXlbhQSEgRSb\n"
        "Nwp1qYXeMrfm9mVoyPEt0MkuU0r5KS0YtOKmrQ6b3kVzMvZgQLKPulTkGNDhymlH\n"
        "Jbb7wpJC42cZWkRJ/EkQdO67pxE9icgA3pZcPC36Wk7E9LnjEqyMoL4EWvouXcDi\n"
        "uDwb3u2gPmjtqYoQuUFopxdpDk9wwMz6oR85ncJjIzJfD+c+33IRpcWSwrNcHLeD\n"
        "4t/HQeH8PLGOzjyQUgyYRcaSeTjBjUQvN68emUtg0WCC3nQ1WINm+O7ASeX53le2\n"
        "ba4WyNiBUwm/Bc/rl6uxPNj6bcJWd3hMeAF8c0NreoHpRKdgj79ru1LcFFqDGN0k\n"
        "b8dMwAQ/AgMBAAECggEAK7IE6a5rj/lNp1jUKln+QbuyvTA+bhrbZChFSyAI92mK\n"
        "REb+DJhjLUBR9jVb9xZN4Mkg+fbAKMtbsh4bIdChcc4ChitUQaM1MqX6DLMWArCa\n"
        "w8PVVyMg6ntz02a0CK+ahJ8XgqLxou+R5JevpkA/MQFu1bOH3HRYL4NoGWv01/K+\n"
        "F1JUMoX/Q1ZFSZ4JK5b01BlcUnXzedQfYprxVPlzXhlRK3f/R2pv4rjc+/Qk04Xj\n"
        "wuUC+VOYTRxA+D5T7S85IhvkGqwAYrHfXoQh66ZZ/XrXiaCTP07lsQZxe/2PTOKQ\n"
        "NXqvNWLGzPRfS1AG9tyh8o7NqrW3VPtZHXYMETPWwQKBgQD8JKTk3KiwNZNrKsVl\n"
        "C0BmbEbGPvTQG3wd6Y1/7kfts1ld2p3tjvjWeZM5owh+cCjM04KLLGfxJqTrTco3\n"
        "B++8V8nnB3QLZ6fLK0YGoyu3EN5mJYarFEHr+sYhj0jF1qTOF9s+q8fBKR+SzWgC\n"
        "/f3NE1/pHADkdge9oE57W7AVwQKBgQDDKjKA7GyHprnghLLNcKX9QNurV9rhRX9R\n"
        "wJRjuVgd/409F+EciX6MGQfxfb1WmIW0OCqLXNpu4QWLB/uergD9IcbHemvCGzEO\n"
        "vaIVUDFVGPlrlHoaP8k1c20gqeQ0tZyIiojd/8fIM/yAzz/rIBDlvMJpIaY4nVDO\n"
        "QX73ddqZ/wKBgFLJfVKfmMtya9rOCk3o6dY3Su2gkiUaQSBZJNqSVA+ZyYflVeFA\n"
        "Bjf1Zv+PpiRNkzfdYolrpR0qwQ95VSHIe1Btyp8OdW2hNRiiNrXLMXlEEqGbRKI7\n"
        "XoGqrw8pyo0asrXwY04qfMXRSMqcosAIwSJqcB1L0ocwsc3zKpGaQzCBAoGBAIcL\n"
        "F88lySNENrIooCCPpgvLaTTyob3m/OZwvz0R7usy/WQcteOtMCd42oo7W1wkRcAi\n"
        "bS3pnivDALbnWzC9bmm5iXZWO2Igtp1gklr1Fm2vzkamgqbPJEccC5Lie8oNMLc6\n"
        "ROJ4LqFVd48Z2Ty0/3w4VL2yTvY1xMgzgctxfUW5AoGAFivRXaSG5EIRWW3RTXy7\n"
        "Fhnwg41TRPG61FJI5uJI1WIIQtrLRv7n3WUaUR/jRf8euM/w/tdpihZGfcYuN8FG\n"
        "RSImuhieNyF6S+jX3e1/QmmP4p6JqQm+cDOpKBfiKophtBtARE2VCrPCJk3jm2dD\n"
        "z3ohqngInqUKqUD+pitnXcY=\n"
        "-----END PRIVATE KEY-----\n";

    std::string const dh =
        "-----BEGIN DH PARAMETERS-----\n"
        "MIIBCAKCAQEA8PtjFY1GCzqPdzc4kqZBMOemKaKVza6QROZI4mrgc0qbzoZwKH6O\n"
        "FHoJ+8cw0o+zBZRqZpU2XU6YFuaypEt2/3XDWLAgUnrMTLqx1+mCTrQwfH2f/Skj\n"
        "8Mk2Q6phjNdAOXwZEhkea6g6f/lrgg8/rirMmfiGPiRgVORdBFPjWHyH5bYuqe/1\n"
        "2+wBatoRoVypRcLI72zJm3l8e0FYGjd8bJJmzbFblptKH9bTuezW2TWA1Fj/piHw\n"
        "XQPEX+Vwj3P16e7uIfqeP2w9GDIkWpnwQJpJTUPmRN3BI3jTAPWorHoBpjlV0Co+\n"
        "UT17EGCXGmnul0gwp45TDKbN65w5VJEBRwIBAg==\n"
        "-----END DH PARAMETERS-----\n";

    ctx.set_options(boost::asio::ssl::context::default_workarounds | boost::asio::ssl::context::no_sslv2 | boost::asio::ssl::context::single_dh_use);

    ctx.use_certificate_chain(boost::asio::buffer(cert.data(), cert.size()));

    ctx.use_private_key(boost::asio::buffer(key.data(), key.size()), boost::asio::ssl::context::file_format::pem);

    ctx.use_tmp_dh(boost::asio::buffer(dh.data(), dh.size()));
}

}  // namespace ws

#endif  // H_2641FB6B_BE30_4EF1_87E8_8C58EE4B718C