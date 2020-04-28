# AES
進階加密標準（Advanced Encryption Standard）縮寫為 **AES**，是最為常見的對稱式加密演算法，是由比利時密碼學家 Joan Daemen 和 Vincent Rijmen 所設計。</br>
本AES加解密程式提供 **ECB、CBC、CTR、OFB、OFB-8、OFB-64、CFB、CFB-8、CFB-64** 九種加密模式，與 **128、192、256** 三種區塊大小與金鑰長度，以及 **No、Zero、Space、ISO/IEC 7816-4、ZeroLen、ANSI X.923、ISO 10126、PKCS7** 八種填充模式。

## 預覽畫面
開始畫面<br>
![開始畫面](/preview/main.png)

自動搜索目錄下的檔案<br>
![加解密選擇檔案](/preview/cryption_selection_file.png)

加密或解密時可以查看進度<br>
![進度條](/preview/encript_progress_bar.gif)

也可同時加解密<br>
![同時加解密](/preview/cript_progress_bar.gif)

各種不同加解密模式<br>
![設定](/preview/setting.png)

可以選擇清空資料夾裡面的檔案<br>
![清空資料夾](/preview/clear_folder.png)

## 尚未完成功能
1. 使用多執行續進行加解密

## 環境配置
本專案採用 Dev-c++ 5.11 開發

#### Dev-c++ 編譯器設定：
```TDM-GCC 4.9.2 64-bit Release```