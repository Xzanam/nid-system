#include <iostream>
#include <pcap.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <cstring>
#include <unordered_map>
#include <fstream>
#include <ctime>

using namespace std;

// Structure to store session information for advanced feature extraction
struct Session {
    int count;
    int srv_count;
    int serror_count;
    int rerror_count;
    time_t first_packet_time;
    time_t last_packet_time;
};

// Track sessions based on source and destination IP/port
unordered_map<string, Session> session_map;

// CSV output file
ofstream csv_file;

// Counter for the number of packets captured
static int packet_counter = 0;
const int max_packets = 5;

// Function to process each packet and write it to CSV
void packet_handler(u_char* args, const struct pcap_pkthdr* header, const u_char* packet) {
    // Increment packet counter
    packet_counter++;
    if (packet_counter > max_packets) {
        pcap_breakloop((pcap_t *)args);
        return;
    }

    struct ip* ip_header = (struct ip*)(packet + 14);  // Assuming Ethernet header is 14 bytes
    int ip_header_len = ip_header->ip_hl * 4;

    // Extract protocol type
    string protocol_type;
    switch (ip_header->ip_p) {
        case IPPROTO_TCP:
            protocol_type = "TCP";
            break;
        case IPPROTO_UDP:
            protocol_type = "UDP";
            break;
        default:
            protocol_type = "OTHER";
    }

    // Extract source and destination IPs
    char src_ip[INET_ADDRSTRLEN], dst_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ip_header->ip_src), src_ip, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(ip_header->ip_dst), dst_ip, INET_ADDRSTRLEN);

    // Create a session key to track this connection
    string session_key = string(src_ip) + ":" + to_string(ntohs(ip_header->ip_id)) + "-" + string(dst_ip);

    // Initialize a session if not present
    if (session_map.find(session_key) == session_map.end()) {
        session_map[session_key] = {0, 0, 0, 0, header->ts.tv_sec, header->ts.tv_sec};
    }

    // Update session duration and other stats
    session_map[session_key].last_packet_time = header->ts.tv_sec;
    int duration = session_map[session_key].last_packet_time - session_map[session_key].first_packet_time;

    // Extract byte counts
    int src_bytes = ntohs(ip_header->ip_len);
    int dst_bytes = ntohs(ip_header->ip_len);  // Same here, for simplicity; adjust based on logic

    // Extract flags (for TCP)
    string flag = "-";
    if (protocol_type == "TCP") {
        struct tcphdr* tcp_header = (struct tcphdr*)(packet + 14 + ip_header_len);
        flag = to_string((int)tcp_header->th_flags);  // Extract TCP flags
    }

    // Check if land attack (same source and destination IP and port)
    bool land = (strcmp(src_ip, dst_ip) == 0) ? true : false;

    // Write data to CSV (duration, protocol_type, src_bytes, dst_bytes, land, flag, etc.)
    csv_file << duration << "," << protocol_type << "," << src_bytes << "," << dst_bytes << ","
             << land << "," << flag << endl;

    // Update session count for session-based features
    session_map[session_key].count++;
}

int main() {
    char error_buffer[PCAP_ERRBUF_SIZE];
    pcap_t* handle;

    // Open live capture on the network interface (adjust for your interface)
    handle = pcap_open_live("wlp2s0", BUFSIZ, 1, 1000, error_buffer);

    if (handle == nullptr) {
        cerr << "Could not open device: " << error_buffer << endl;
        return 1;
    }

    // Open CSV file to write output
    csv_file.open("packet_capture.csv");

    // Write CSV header
    csv_file << "duration,protocol_type,src_bytes,dst_bytes,land,flag" << endl;

    // Start packet capture loop
    pcap_loop(handle, 0, packet_handler, (u_char *)handle);

    // Close the session and CSV file
    pcap_close(handle);
    csv_file.close();

    return 0;
}
