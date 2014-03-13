#include <iostream>
#include <fstream>
#include <iterator>
#include <iomanip>

#define BOOST_ASIO_SEPARATE_COMPILATION TRUE
//        BOOST_ASIO_DYN_LINK



//define TORRENT_VERBOSE_LOGGING 1
#define TORRENT_DEBUG 1
#define TORRENT_LOGGING 1
//define TORRENT_USE_IOSTREAM 1





#include <libtorrent/entry.hpp>
#include <libtorrent/session.hpp>
//#include <libtorrent/torrent_info.hpp>
//#include <libtorrent/fingerprint.hpp>

//#include <libtorrent/bencode.hpp>
//#include <libtorrent/lazy_entry.hpp>

//#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
//#include "boost"

int main(int argc, char * argv[])
{
    using namespace libtorrent;

//    if (argc != 2)
//    {
//        std::cerr << "usage: ./simple_client torrent-file\n"
//                "to stop the client, press return.\n";
//        return 1;
//    }

#ifndef BOOST_NO_EXCEPTIONS
    try
#endif
    {
        session s;
        error_code err;

        s.listen_on(std::make_pair(8880, 8899), err);
        if (err) std::cerr << err.message();

        std::cout << "listens on " << s.listen_port() << ", SSL on " << s.ssl_listen_port() << "\n";

        add_torrent_params p;
        p.save_path = "./downloads";
//        p.ti = new torrent_info("magnet:?xt=urn:btih:67517aa1272ab782faa506b86b244c873cbb6515");
//        p.url = argv[1];
//        p.url = "magnet:?xt=urn:btih:67517aa1272ab782faa506b86b244c873cbb6515&dn=rutor.org&tr=udp://bt.rutor.org:2710&tr=http://retracker.local/announce";
        p.url = "magnet:?xt=urn:btih:2fe6343a61d4edc1d4dcbdbc31b786a32c6a6e15";
        s.add_torrent(p, err);
        if (err)
            std::cerr << "Add torrent failed: " << err.message();

        char a;
        while (a != 'q')
        {
            entry st = s.dht_state();
//            st.print(std::cout);

            int nodes_count = 0;
            if (st.find_key("nodes"))
            {
                entry::list_type &nodes = st.find_key("nodes")->list();
                nodes_count = nodes.size();
            }

            std::cout << "DHT=" << (s.is_dht_running() ? "Y" : "N");

            std::vector<torrent_handle> lh = s.get_torrents();
            int _count = 0;
            for (std::vector<torrent_handle>::iterator i = lh.begin();
                i != lh.end();
                i++, _count++)
            {
                torrent_handle th = *i;
                torrent_status ts = th.status();
                std::cout << " seeds=" << ts.num_seeds << " peers=" << ts.num_peers;
                std::cout << " state=" << ts.state << " md=" << (ts.has_metadata ? "Y" : "N");
                std::cout << " nodes=" << nodes_count;

                if (ts.has_metadata)
                {
                    try
                    {
                        torrent_info ti = th.get_torrent_info();
                    }
                    catch (std::exception& e)
                    {
                        std::cerr << "torrent_info: " << e.what() << "\n";
                    }
                }
            }

            std::cin.unsetf(std::ios_base::skipws);
            std::cin >> a;

            if (a == 'r')
            {
                s.add_dht_router(std::make_pair(std::string("router.bittorrent.com"), 6881));
                s.add_dht_router(std::make_pair(std::string("router.utorrent.com"), 6881));
                s.add_dht_router(std::make_pair(std::string("dht.transmissionbt.com"), 6881));
                s.add_dht_router(std::make_pair(std::string("dht.aelitis.com"), 6881)); // Vuze

                std::cerr << "\nrouters added\n";
            }

            if (a == 'a')
            {
                s.start_lsd();
                s.start_upnp();
                s.start_natpmp();
            }
        }
        s.abort();
    }
#ifndef BOOST_NO_EXCEPTIONS
    catch (std::exception& e)
    {
            std::cerr << e.what() << "\n";
    }
#endif
    return 0;
}