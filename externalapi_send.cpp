#include <QJsonArray>
#include <QMessageBox>
#include "externalapi_send.hpp"

/// @brief Send a start ot stop cmd to the external API.
/// @param db pointer to a QSqlDatabase
/// @param net_manager the QNetworkAccessManager as parameter to be able to use signals.
/// @param send_topic send either `start` or `stop` to external api
void send_cmd(QSharedPointer<QSqlDatabase> &db, QSharedPointer<QNetworkAccessManager> &net_manager, const SendTopic &send_topic)
{
    auto base_url = get_base_url_external_api_base_url(db);
    
    if (!base_url.base_url.isEmpty())
    {
        auto api_token = get_external_api_token_by_base_url(db, base_url.pk);
        auto api_data = get_external_api_data_by_base_url(db, base_url.pk);
        
        if (!api_token.is_empty() && !api_data.is_empty())
        {
            QJsonValue json_val;
            QJsonArray json_val_arr;
            QString request_data {};
            
            QNetworkRequest net_req {base_url.base_url + api_data.start_api_path};
            net_req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
            
            auto json_doc = QJsonDocument::fromJson(QByteArray::fromStdString(api_data.req_data.toStdString()));
            if (json_doc.isNull() || json_doc.isEmpty())
            {
                return;
            }
            
            auto json_obj = json_doc.object();
            request_data += api_token.kind.toLower() + "=" + api_token.token + "&";
            
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
            
            if (json_val != QJsonValue::Undefined)
            {
                for (auto val : json_val_arr)
                {
                    request_data += val.toString() + "&";
                }
                // removing trailing `&`
                request_data = request_data.sliced(0, request_data.size() - 1);
            }
            
            net_manager->post(net_req, QByteArray::fromStdString(request_data.toStdString()));
        }
    }
}
