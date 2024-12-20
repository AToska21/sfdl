#include "Downloader.hpp"

namespace CurlTools
{
    struct Memory
    {
        char *response;
        size_t size;
    };

    static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
    {
        size_t realsize = size * nmemb;
        struct Memory *mem = (struct Memory *)userp;

        mem->response = (char*)realloc(mem->response, mem->size + realsize + 1);
        if (mem->response == NULL)
        {
            /* out of memory! */
            ScrUtils::printf("not enough memory (realloc returned NULL)\n");
            return 0;
        }

        memcpy(&(mem->response[mem->size]), contents, realsize);
        mem->size += realsize;
        mem->response[mem->size] = 0;

        return realsize;
    }

    // Write file callback
    static size_t WriteFileCallback(void *contents, size_t size, size_t nmemb, FILE *userp)
    {
        return fwrite(contents, size, nmemb, userp);
    }

    static int DebugCallback(CURL *handle,
                             curl_infotype type,
                             char *data,
                             size_t size,
                             void *userp)
    {
        switch (type)
        {
        case CURLINFO_TEXT:
            ScrUtils::printf("== Info: %s\n", data);
            break;
        case CURLINFO_HEADER_OUT:
            ScrUtils::printf("=> Send header\n");
            break;
        case CURLINFO_DATA_OUT:
            ScrUtils::printf("=> Send data\n");
            break;
        case CURLINFO_SSL_DATA_OUT:
            ScrUtils::printf("=> Send SSL data\n");
            break;
        case CURLINFO_HEADER_IN:
            ScrUtils::printf("<= Recv header\n");
            break;
        case CURLINFO_DATA_IN:
            ScrUtils::printf("<= Recv data\n");
            break;
        case CURLINFO_SSL_DATA_IN:
            ScrUtils::printf("<= Recv SSL data\n");
            break;
        case CURLINFO_END:
            ScrUtils::printf("Data End\n");
            break;
        }
        return 0;
    }
}




namespace Tools
{
    Downloader::Downloader()
    {
        m_maxStringSize = 2 * 1024; // 2 KB
    }

    Downloader::~Downloader()
    {
    }


    unsigned int Downloader::GetMaxStringSize() const
    {
        return m_maxStringSize;
    }

    void Downloader::SetMaxStringSize(unsigned int maxStringSize)
    {
        m_maxStringSize = maxStringSize;
    }


    int Downloader::DownloadToFile(std::string url, std::string fileName)
    {
        // Open file for writing
        FILE *fp = fopen(fileName.c_str(), "wb");
        if (fp == NULL)
        {
            ScrUtils::printf("Error opening file %s\n", fileName.c_str());
            return -1;
        }

        // Init curl
        CURL *curl = curl_easy_init();
        CURLcode res;
        if (!curl)
        {
            ScrUtils::printf("Error initializing curl\n");
            return -1;
        }

        // Set curl options
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlTools::WriteFileCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)fp);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

        // Perform curl
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            ScrUtils::printf("Error performing curl\n");
            fclose(fp);
            return -1;
        }

        // Cleanup curl
        curl_easy_cleanup(curl);

        // Close file
        fclose(fp);

        return 0;
    }

    int Downloader::DownloadToString(std::string url, std::string &str)
    {
        // Init curl
        CURL *curl = curl_easy_init();
        CURLcode res;

        if (!curl)
        {
            ScrUtils::printf("Error initializing curl\n");
            return -1;
        }

        CurlTools::Memory chunk;
        chunk.response = (char*)malloc(1);
        chunk.size = 0;

        // Set curl options
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlTools::WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
        // Set max string size
        curl_easy_setopt(curl, CURLOPT_MAXFILESIZE, m_maxStringSize);

        // Wii U SSL stuff
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

        // Perform curl
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            ScrUtils::printf("Error performing curl\n");
            return -1;
        }

        // Cleanup curl
        curl_easy_cleanup(curl);

        // Perform string copy
        str.clear();
        str.append(chunk.response, chunk.size < m_maxStringSize ? chunk.size : m_maxStringSize); // Uses m_maxStringSize as string size if chunk.size is bigger than it

        // Free memory
        free(chunk.response);

        return 0;
    }
}