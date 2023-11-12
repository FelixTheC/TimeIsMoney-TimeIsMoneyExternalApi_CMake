#include "externalapi_send.hpp"

QNetworkRequest get_network_request(QString &&url, const ContentType &type) noexcept
{
    QNetworkRequest net_req {url};
    
    switch (type)
    {
        case ContentType::form_urlencoded:
            net_req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
            break;
        case ContentType::form_data:
            net_req.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/form-data");
            break;
        case ContentType::binary:
            net_req.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
            break;
        case ContentType::raw_json:
            net_req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
            break;
        case ContentType::raw_text:
            net_req.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");
            break;
        case ContentType::raw_xml:
            net_req.setHeader(QNetworkRequest::ContentTypeHeader, "application/xhtml+xml");
            break;
    }
    
    return net_req;
}

QByteArray get_request_data(QJsonValue &&json_val,
                            QNetworkRequest &net_req,
                            const ContentType &type,
                            const std::tuple<const Auth, const std::string> &api_kind,
                            const std::string &token)
{
    bool body_auth = false;
    std::string req_body {};
    
    auto auth = std::get<0>(api_kind);
    auto auth_name = std::get<1>(api_kind);
    
    if (auth != Auth::None)
    {
        net_req.setRawHeader(QByteArray::fromStdString("Authorization"),
                             QByteArray::fromStdString(auth_repr(auth) + " " + token));
    }
    else
    {
        body_auth = true;
    }
    
    switch (type)
    {
        case ContentType::form_data:
        case ContentType::form_urlencoded:
            if (body_auth)
            {
                req_body += auth_name + "=" + token + "&";
            }
            
            if (json_val != QJsonValue::Undefined)
            {
                if (json_val.isArray())
                {
                    for (auto val : json_val.toArray())
                    {
                        req_body += val.toString().toStdString() + "&";
                    }
                }
                else if (json_val.isObject())
                {
                    for (const auto &key : json_val.toObject().keys())
                    {
                        auto val = json_val[key];
                        req_body += key.toStdString() + "=" + val.toString().toStdString() + "&";
                    }
                }
                // removing trailing `&`
                req_body = req_body.substr(0, req_body.size() - 1);
            }
            break;
        case ContentType::raw_json:
            if (json_val != QJsonValue::Undefined)
            {
                auto record = json_val.toObject();
                
                if (body_auth)
                {
                    record.insert(QString::fromStdString(auth_name), QJsonValue::fromVariant(QString::fromStdString(token)));
                }
                
                QJsonDocument json_obj(record);
                req_body = json_obj.toJson(QJsonDocument::Compact).toStdString();
            }
            break;
        case ContentType::raw_text:
            if (body_auth)
            {
                req_body += auth_name + " " + token + " ";
            }
            
            if (json_val != QJsonValue::Undefined)
            {
                if (json_val.isArray())
                {
                    for (auto val: json_val.toArray())
                    {
                        req_body += val.toString().toStdString() + " ";
                    }
                }
                else if (json_val.isObject())
                {
                    for (const auto &key : json_val.toObject().keys())
                    {
                        auto val = json_val[key];
                        req_body += key.toStdString() + " " + val.toString().toStdString() + " ";
                    }
                }
                // removing trailing whitespace
                req_body = req_body.substr(0, req_body.size() - 1);
            }
            break;
        case ContentType::binary:
        case ContentType::raw_xml:
            break;
    }
    
    return QByteArray::fromStdString(req_body);
}

[[ nodiscard ]] std::tuple<const Auth, const std::string> get_api_kind(const std::string &api_kind_str)
{
    if (api_kind_str == "basic")
    {
        return {Auth::Basic, api_kind_str};
    }
    else if (api_kind_str == "bearer")
    {
        return {Auth::Bearer, api_kind_str};
    }
    else
    {
        return {Auth::None, api_kind_str};
    }
}

/// @brief Send a start ot stop cmd to the external API.
/// @param db pointer to a QSqlDatabase
/// @param net_manager the QNetworkAccessManager as parameter to be able to use signals.
/// @param send_topic send either `start`, `pause` or `stop` to external api
/// @param type the header type of the request
void send_cmd(QSharedPointer<QSqlDatabase> &db,
              QSharedPointer<QNetworkAccessManager> &net_manager,
              const SendTopic &send_topic,
              const ContentType &type)
{
    auto base_url = get_base_url_external_api_base_url(db);
    
    if (!base_url.base_url.isEmpty())
    {
        auto api_token = get_external_api_token_by_base_url(db, base_url.pk);
        auto api_data = get_external_api_data_by_base_url(db, base_url.pk);
        
        if (!api_token.is_empty() && !api_data.is_empty())
        {
            QJsonValue json_val;
            
            auto net_req = get_network_request(base_url.base_url + api_data.start_api_path, type);
            
            auto json_doc = QJsonDocument::fromJson(QByteArray::fromStdString(api_data.req_data.toStdString()));
            if (json_doc.isNull() || json_doc.isEmpty())
            {
                return;
            }
            
            auto json_obj = json_doc.object();
            switch (send_topic)
            {
                case SendTopic::Start:
                    json_val = json_obj.value("start");
                    break;
                case SendTopic::Pause:
                    json_val = json_obj.value("pause");
                    break;
                case SendTopic::Stop:
                    json_val = json_obj.value("stop");
                    break;
            }
            
            const auto api_kind_name = api_token.kind.toLower().toStdString();
            const auto api_kind = get_api_kind(api_kind_name);
            
            auto req_body = get_request_data(std::move(json_val),
                                             net_req,
                                             type,
                                             api_kind,
                                             api_token.token.toStdString());
            
            net_manager->post(net_req, req_body);
        }
    }
}
