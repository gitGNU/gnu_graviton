/**
 * @file
 *
 * @author  Sina Hatef Matbue ( _null_ ) <sinahatef.cpp@gmail.com>
 *
 * @section License
 * This file is part of GraVitoN.
 *
 * Graviton is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Graviton is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Graviton.  If not, see http://www.gnu.org/licenses/.
 *
 * @brief GraVitoN socks5 client
 *
 */

#ifndef GRAVITON_SOCKS5_CLIENT_H
#define GRAVITON_SOCKS5_CLIENT_H

#include <graviton.hpp>
#include <core/memory.hpp>
#include <utils/socks5.hpp>
#include <core/memory.hpp>
#include <core/tcp_client.hpp>

namespace GraVitoN
{
    namespace Utils
    {

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-//
        /*
         * socks5 client class
         * socks5 connect + socks5 authentication
         * call send/sendString to send data and recv/recvString to recieve data
         *
         */
        class SOCKS5_Client : public Core::TCP_Client
        {
        protected:
            unsigned int remote_port;
            string remote_host;
            string username;
            string pass;
            unsigned char cmd;

            bool auth_mod;

        protected:
            virtual bool greeting();
            virtual bool authenticate();
            virtual bool askConnect();
            virtual bool askBind();
            virtual bool askUDP();

        public:
            SOCKS5_Client(
                const string &_socks_server_ip,
                const unsigned int _port,
                const string _remote_host,
                const unsigned _remote_port,
                const string &_username = "",
                const string &_pass = "",
                const unsigned char _socks_type = SOCKS5::CMD_CONNECT):
                TCP_Client(_socks_server_ip, _port)
                {
                    username = _username;
                    pass = _pass;
                    cmd = _socks_type;

                    remote_host = _remote_host;
                    remote_port = _remote_port;
                }

            ~SOCKS5_Client()throw()
                {

                }

            virtual bool connect();
        };

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-//
        bool SOCKS5_Client::connect()
        {
            Core::Logger::logItLn("[SOCKS5 Client] Connecting...");
            if( TCP_Client::connect() )
            {
                if( !greeting() )
                    return false;

                if( auth_mod )
                    if( !authenticate() )
                        return false;

                /// @todo bind, udp
                if( cmd == SOCKS5::CMD_CONNECT )
                    return askConnect();

                return true;
            }
            return false;
        }

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-//
        bool SOCKS5_Client::greeting()
        {
            Core::Logger::logItLn("[SOCKS5 Client] Greeting...");

            Core::Memory<guchar> data(5);
            size_t sz = 0;

            data[sz++] = SOCKS5::VER;
            data[sz++] = 0x02; // 2 auth methods
            data[sz++] = SOCKS5::AUTH_NONE;
            data[sz++] = SOCKS5::AUTH_USERPASS;

            if( TCP_Client::send(data) )
            {
                if( TCP_Client::recv(data) )
                {
                    if( sz <= 1 || data[1] == SOCKS5::REP_AUTH_UNACCEPTABLE )
                        return false;
                    auth_mod = (data[1] == SOCKS5::AUTH_USERPASS) && (!username.empty() || !pass.empty());
                    cout << "AUTH MOD: " << auth_mod << endl;
                    Core::Logger::logItLn("[SOCKS5 Client] Done");
                    return true;
                }

                return false;
            }
            return false;
        }

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-//
        bool SOCKS5_Client::authenticate()
        {
            Core::Logger::logItLn("[SOCKS5 Client] Authenticating...");
            // unsigned char *data = new unsigned char[3 + username.size() + pass.size() + 1];
            Core::Memory<guchar> data(3 + username.size() + pass.size() + 1);
            size_t sz = 0;

            data[sz++] = SOCKS5::AUTH_VER;
            data[sz++] = (unsigned char)username.size();
            for(size_t i=0; i<username.size(); ++i)
                data[sz++] = (unsigned char)username[i];
            data[sz++] = (unsigned char)pass.size();
            for(size_t i=0; i<pass.size(); ++i)
                data[sz++] = (unsigned char)pass[i];

            if( TCP_Client::send(data) )
            {
                if( TCP_Client::recv(data) )
                {
                    if( sz <= 1 || data[1] != SOCKS5::REP_SUCCEEDED )
                    {
                        Core::Logger::logItLn("[SOCKS5 Client] FAILED");
                        cout << "size: " << sz << ", DATA: " << (int)data[1] << endl;
                        return false;
                    }
                    Core::Logger::logItLn("[SOCKS5 Client] Done");
                    return true;
                }
                Core::Logger::logItLn("[SOCKS5 Client] NO ANSWER");
                return false;
            }
            return false;
        }

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-//
        bool SOCKS5_Client::askConnect()
        {
            Core::Logger::logItLn("[SOCKS5 Client] Ask CONNECT...");

            // unsigned char *data = new unsigned char[5];
            Core::Memory<guchar> data(32);
            size_t sz = 0;

            data[sz++] = SOCKS5::VER;
            data[sz++] = SOCKS5::CMD_CONNECT;
            data[sz++] = SOCKS5::RSV;

            /// @todo IPv6
            if( Core::Socket::isAValidIPv4(remote_host) )
            {
                data[sz++] = SOCKS5::ATYP_IPV4;

                unsigned int hip = Core::Socket::hostToNet_32( Core::Socket::strToHexIPv4(remote_host) );
                // memcpy((void*)(data+sz), (void*)&hip, 4);
                data.copy((guchar*)&hip, 4, sz);
                sz += 4;
            }
            else
            {
                data[sz++] = SOCKS5::ATYP_DOMAINNAME;

                size_t len = remote_host.size();
                data[sz++] = (unsigned char)len;
                // memcpy((void*)(data+sz), (void*)remote_host.c_str(), len);
                data.copy((guchar*)remote_host.c_str(), len, sz);
                sz += len;
            }

            unsigned int hport = Core::Socket::hostToNet_16( remote_port );
            // memcpy((void*)(data+sz), (void*)&hport, 2);
            data.copy((guchar*)&hport, 2, sz);
            sz += 2;

            if( TCP_Client::send(data) )
            {
                // cout << "sending..." << endl;
                if( TCP_Client::recv(data) )
                {
                    // cout << "recv..." << endl;
                    if( sz <= 1 || data[1] != SOCKS5::REP_SUCCEEDED )
                    {
                        Core::Logger::logItLn("[SOCKS5 CLIENT] [FAILD]");
                        return false;
                    }
                    return true;
                }

                return false;
            }
            return false;
        }

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-//
/// @todo Request BIND
        bool SOCKS5_Client::askBind()
        {
            Core::Logger::logItLn("[SOCKS5 Client] Ask BIND...");
        }

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-//
/// @todo Request UDP
        bool SOCKS5_Client::askUDP()
        {
            Core::Logger::logItLn("[SOCKS5 Client] Ask UDP...");
        }

    }
}
#endif
