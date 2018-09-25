
これは何？
--

リンカです。今のところルネサスRXマイコンのELFオブジェクトに対応しています。
コンパイラ・開発環境としてCS+を想定しています。
(2018/9/20現在)


使い方
--
1).setup.shを実行します。
./setup.sh

setup.sh ではRubyの拡張モジュールのビルドと配置を行います。
rubyがビルドできる環境があれば拡張モジュールはビルドできるはずです。
Debian/Ubuntu系の環境だと、
$sudo apt install ruby-dev
でrubyのビルドに必要なファイルがインストールされます。

正常にビルドが終了するとelf32.soというライブラリが生成され、ELFフォルダに配置されます。
リンカを動かすのに必要な準備は以上です。

2).リンカの実行
app.rbがエントリポイントです。
コマンドライン引数で .clinkファイルを指定すると .clinkファイルの内容に従ってリンクを行います。
実行例)
./app test/led/two_files/

リンクが終了すると、.clinkで指定したファイル名が出力されます。

3).書き込み
リンクしたファイル(.abs)をE1エミュレータ等をターゲットボードに書き込み、プログラムを実行してください。

サンプルについて
--
test/led フォルダ以下にGR-SAKURAで動作確認をしたサンプルプロジェクト一式があります。
https://github.com/simotin13/rb_linker/tree/master/test/led

- one_file
resetprg.objのみをリンクします。

- two_files
resetprg.objとmain.objをリンクします。
