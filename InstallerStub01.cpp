
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <curl/curl.h>
#include "contrib/minizip/zip.h"
#include "contrib/minizip/unzip.h"
#include <direct.h>


// Callback function for curl to write downloaded data to file
static size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream) {
    size_t written = fwrite(ptr, size, nmemb, (FILE*)stream);
    return written;
}

// Callback function for curl to display download progress
static int progress_func(void* ptr, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
    std::cout << "Downloaded " << dlnow << " of " << dltotal << " bytes.\n";
    return 0;
}

// Function to download file from given url to specified path
bool downloadFile(const std::string& url, const std::string& path) {
    CURL* curl;
    FILE* fp = NULL;
    CURLcode res;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        FILE* fp = NULL;
        fopen_s(&fp, path.c_str(), "wb");
        if (!fp) {
            std::cerr << "Could not open file: " << path << '\n';
            curl_easy_cleanup(curl);
            return false;
        }
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_func);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36");
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // Disable certificate verification
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); // Disable hostname verification
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "Error downloading file: " << curl_easy_strerror(res) << '\n';
            fclose(fp);
            curl_easy_cleanup(curl);
            return false;
        }
        fclose(fp);
        curl_easy_cleanup(curl);
        return true;
    }
    return false;
}

// 解压缩 ZIP 文件并保存到指定目录
bool extractZip(const std::string& zipPath, const std::string& extractPath) {
    unzFile zipFile = unzOpen(zipPath.c_str());
    if (!zipFile) {
        std::cerr << "Could not open ZIP file: " << zipPath << '\n';
        return false;
    }
    unz_global_info globalInfo;
    if (unzGetGlobalInfo(zipFile, &globalInfo) != UNZ_OK) {
        std::cerr << "Could not get global ZIP info.\n";
        unzClose(zipFile);
        return false;
    }
    char buffer[1024];
    for (uLong i = 0; i < globalInfo.number_entry; i++) {
        unz_file_info fileInfo;
        if (unzGetCurrentFileInfo(zipFile, &fileInfo, buffer, sizeof(buffer), NULL, 0, NULL, 0) != UNZ_OK) {
            std::cerr << "Could not get file info for file #" << i << ".\n";
            unzClose(zipFile);
            return false;
        }
        std::string entryName(buffer, fileInfo.size_filename);
        if (unzOpenCurrentFile(zipFile) != UNZ_OK) {
            std::cerr << "Could not open file #" << i << " in ZIP.\n";
            unzClose(zipFile);
            return false;
        }

        std::string extractFilePath = extractPath + "/" + entryName;
        if (entryName.back() == '/') {
            // 如果是目录，则创建目录
            if (_mkdir(extractFilePath.c_str()) == -1) {
                std::cerr << "Could not create directory: " << extractFilePath << '\n';
                unzCloseCurrentFile(zipFile);
                unzClose(zipFile);
                return false;
            }
        }
        else {
            // 如果是文件，则创建文件并写入数据
            std::ofstream outFile(extractFilePath, std::ios::binary);
            if (!outFile) {
                std::cerr << "Could not create file: " << extractFilePath << '\n';
                unzCloseCurrentFile(zipFile);
                unzClose(zipFile);
                return false;
            }
            int result = UNZ_OK;
            do {
                result = unzReadCurrentFile(zipFile, buffer, sizeof(buffer));
                if (result < 0) {
                    std::cerr << "Error " << result << " while reading file #" << i << " in ZIP.\n";
                    unzCloseCurrentFile(zipFile);
                    unzClose(zipFile);
                    return false;
                }
                if (result > 0) {
                    outFile.write(buffer, result);
                }
            } while (result > 0);
            outFile.close();
        }
        unzCloseCurrentFile(zipFile);
        if (unzGoToNextFile(zipFile) != UNZ_OK) {
            break;
        }
    }
    unzClose(zipFile);
    std::cout << "Extract succeeded.\n";
    return true;
}


int main() {
    // 下载 ZIP 文件到临时目录
    std::string url = "https://example.com/package.zip";
    char zipPath[MAX_PATH];
    tmpnam_s(zipPath, MAX_PATH);
    if (downloadFile(url, zipPath)) {
        // 解压缩 ZIP 文件到指定目录
        std::string extractPath = "/path/to/extract";
        
        if (extractZip(zipPath, extractPath)) {
            // 删除临时文件
            remove(zipPath);
            return 0;
        }
        
    }
    // 如果发生错误，则退出程序并返回非零错误代码
    return 1;
}



