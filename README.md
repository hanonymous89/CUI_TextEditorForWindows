# CUI_TextEditorForWindows  
# 使い方
日本語すらできないので英語に翻訳はやめておきます。  
環境変数に追加すると使いやすいです。  
>cmdLine\>editor.exe fileName codePage  
>
fileNameにはファイル名を入れてください。  
codePageにはコードページを入れてください。  
入力されない場合はデフォルト値になります。  
例  
test.txtをunicodeで開きたい場合  
>cmdLine\>editor.exe test.txt 65001
>
# 編集中コマンド
編集中コマンドを使用する場合はescキーを押してから以下の入力(ここにある文字->説明)すると実行されます。  
入力はコンソールのタイトルバーに表示されます。(enterで決定)  
q->quit(終了)  
r->read(ファイル読み込み) 
>read\>fileName codePage  
>
s->save(保存)  
もしファイルが設定されていない場合入力を求められます。  
>fileName\>fineName
>
o->option(オプション)  
コマンドラインと同様に使います  
>fileName\>fineName codePage
>
f->find(検索)  
現在の状態から文字列を検索します。  
>find\>findWord
>
# 現在確認されているバグ
・コンソールをリサイズするとスクロールバーが消えてしまう  
・"も"を入力すると次の文字が入力されるまで表示されない  
|-具体的には244(カーソル移動)の判定とかぶってしまっている  
・サイズが大きくなると表示されない   
