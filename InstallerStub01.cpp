// InstallerStub01.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <curl/curl.h>
#include "minizip/zip.h"
#include "minizip/unzip.h"

// 输出进度条
void progressFunc(double dltotal, double dlnow, double ultotal, double ulnow) {
    std::cout << "Download progress: " << static_cast<int>(dlnow * 100 / dltotal) << "%\r";
    std::cout.flush();
}

// 下载文件并保存到指定路径
bool downloadFile(const std::string& url, const std::string& path) {
    CURL* curl = curl_easy_init();
    if (curl) {
        FILE* file = fopen(path.c_str(), "wb");
        if (file) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, false);
            curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progressFunc);
            CURLcode res = curl_easy_perform(curl);
            fclose(file);
            curl_easy_cleanup(curl);
            if (res == CURLE_OK) {
                std::cout << "Download succeeded.\n";
                return true;
            }
            else {
                std::cerr << "Download failed: " << curl_easy_strerror(res) << '\n';
            }
        }
        else {
            std::cerr << "Could not open file: " << path << '\n';
        }
    }
    else {
        std::cerr << "Could not initialize curl.\n";
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




        yy20190811@126.com
            继续
            c
            Copy code
            std::string extractFilePath = extractPath + "/" + entryName;
        if (entryName.back() == '/') {
            // 如果是目录，则创建目录
            if (mkdir(extractFilePath.c_str(), S_IRWXU | S_IRWXG | S_IRWXO) == -1) {
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
    std::string zipPath = std::tmpnam(nullptr);
    if (downloadFile(url, zipPath)) {
        // 解压缩 ZIP 文件到指定目录
        std::string extractPath = "/path/to/extract";
        if (extractZip(zipPath, extractPath)) {
            // 删除临时文件
            remove(zipPath.c_str());
            return 0;
        }
    }
    // 如果发生错误，则退出程序并返回非零错误代码
    return 1;
}







int main()
{
    std::cout << "Hello World!\n";
}

