#include "gtest/gtest.h"
#include "externalapi_send.hpp"

TEST(ApiSend, get_network_request_headers_form_urlencoded)
{
    auto net_req = get_network_request("https://example.net", ContentType::form_urlencoded);
    ASSERT_EQ(net_req.header(QNetworkRequest::ContentTypeHeader), "application/x-www-form-urlencoded");
}

TEST(ApiSend, get_network_request_headers_form_data)
{
    auto net_req = get_network_request("https://example.net", ContentType::form_data);
    ASSERT_EQ(net_req.header(QNetworkRequest::ContentTypeHeader), "multipart/form-data");
}

TEST(ApiSend, get_network_request_headers_binary)
{
    auto net_req = get_network_request("https://example.net", ContentType::binary);
    ASSERT_EQ(net_req.header(QNetworkRequest::ContentTypeHeader), "application/octet-stream");
}

TEST(ApiSend, get_network_request_headers_json)
{
    auto net_req = get_network_request("https://example.net", ContentType::raw_json);
    ASSERT_EQ(net_req.header(QNetworkRequest::ContentTypeHeader), "application/json");
}

TEST(ApiSend, get_network_request_headers_text)
{
    auto net_req = get_network_request("https://example.net", ContentType::raw_text);
    ASSERT_EQ(net_req.header(QNetworkRequest::ContentTypeHeader), "text/plain");
}

TEST(ApiSend, get_network_request_headers_xml)
{
    auto net_req = get_network_request("https://example.net", ContentType::raw_xml);
    ASSERT_EQ(net_req.header(QNetworkRequest::ContentTypeHeader), "application/xhtml+xml");
}

TEST(ApiSend, create_json_request_body)
{
    QJsonValue test_json_obj = QJsonValue(QJsonObject({{"foo", "bar"}, {"val", "foobar"}}));
    
    auto net_req = get_network_request("https://example.net", ContentType::raw_json);
    
    auto req_body = get_request_data(std::move(test_json_obj),
                                     net_req,
                                     ContentType::raw_json,
                                     {Auth::None, "auth"},
                                     "123456");

    ASSERT_EQ(req_body.toStdString(), "{\"auth\":\"123456\",\"foo\":\"bar\",\"val\":\"foobar\"}");
}

TEST(ApiSend, create_json_request_body_with_invalid_json)
{
    QJsonValue test_json_arr = QJsonValue(QJsonArray({"foo=bar", "val=foobar"}));
    auto net_req = get_network_request("https://example.net", ContentType::raw_json);
    
    auto req_body = get_request_data(std::move(test_json_arr),
                                     net_req,
                                     ContentType::raw_json,
                                     {Auth::None, "auth"},
                                     "123456");
    
    ASSERT_EQ(req_body.toStdString(), "{\"auth\":\"123456\"}");
}

TEST(ApiSend, create_form_request_body)
{
    QJsonValue test_json_obj = QJsonValue(QJsonObject({{"foo", "bar"}, {"val", "foobar"}}));
    QJsonValue test_json_arr = QJsonValue(QJsonArray({"foo=bar", "val=foobar"}));
    
    auto net_req = get_network_request("https://example.net", ContentType::form_urlencoded);
    
    {
        auto req_body = get_request_data(std::move(test_json_obj),
                                         net_req,
                                         ContentType::form_urlencoded,
                                         {Auth::None, "auth"},
                                         "123456");
        
        ASSERT_EQ(req_body.toStdString(), "auth=123456&foo=bar&val=foobar");
    }
    
    {
        auto req_body = get_request_data(std::move(test_json_arr),
                                         net_req,
                                         ContentType::form_urlencoded,
                                         {Auth::None, "auth"},
                                         "123456");
        
        ASSERT_EQ(req_body.toStdString(), "auth=123456&foo=bar&val=foobar");
    }
}

TEST(ApiSend, get_api_kind)
{
    {
        auto expected = std::make_tuple<Auth, std::string>(Auth::Basic, "basic");
        ASSERT_EQ(get_api_kind("basic"), expected);
    }
    {
        auto expected = std::make_tuple<Auth, std::string>(Auth::Bearer, "bearer");
        ASSERT_EQ(get_api_kind("bearer"), expected);
    }
    {
        auto expected = std::make_tuple<Auth, std::string>(Auth::None, "auth");
        ASSERT_EQ(get_api_kind("auth"), expected);
    }
    {
        auto expected = std::make_tuple<Auth, std::string>(Auth::None, "unknown");
        ASSERT_EQ(get_api_kind("unknown"), expected);
    }
}
