# @esc addon for Fcitx

@esc は、
Vim ライクなキーバインド用に、esc キー押下で IME をオフにさせるための
Fcitx 用 addon です.


## Installation

cmake で C言語用のビルドができる環境であれば、以下の手順でインストールできると思います(Arch Linux で確認しています).

```
$ git clone https://github.com/hankei6km/fcitx-at-esc.git
$ cd fcitx-at-esc
$ cmake -DCMAKE_INSTALL_PREFIX=/usr .
$ sudo make install
```

## Usage

@esc が有効な状態で日本語入力を行っているとき、`esc` を押下することで IME がオフ(disabel) になります.
なお、変換途中の文字ある場合の `esc` の押下については、
後述の設定によって変化します
(デフォルトでは、`esc` の押下は IME 側で処理されます).

## Configuration

Fcitx の設定パネルから「アドオン」タブを開き「@esc」項目を選択すると、以下の項目を設定できます.

### 有効化(デフォルト値はオン)

@esc の機能の有効化/無効化を切り替えます.
なお、この項目は挙動を変更するだけで、addon 自体の有効化/無効化には反映されません.

### 強制的にIMEをオフ(デフォルト値はオフ)

この項目をオンにしていると、「変換入力中か？」といった状態に関係なく IME をオフにします.
変換途中の未確定文字の扱いについては、後述の「未確定文字をコミットする」項目の設定に依存します.

なお、この項目をオンにしていると
IME に対しての `esc` 押下による操作が行えなくなります.
これについては、IME の設定等で他のキーに `esc` 相当の機能を割り当てる、
といった回避方法が考えられます.

### 未確定文字をコミットする(デフォルト値はオフ)

上記の「強制的にIMEをオフ」と、この項目をオンにしていると、
`esc` 押下時に未確定の文字があればコミット(確定)します.
なお、チェックを外しても IME や Fcitx の設定によってはコミットされるかもしれません.

この項目がオンの時の挙動が少しわかりにくいのですが、
例えば Vim 上で変換入力している場合に `esc` を押下すると「未確定文字がコミット(確定)され IME はオフになり、Vim はNORMAL モードに切り替わる」といった動作となります.

### 有効化/無効化の切り替え(デフォルト値は空)

@esc の機能の有効化と無効化を切り替えるホットキーを指定できます.

## Known Issues

現状では mozc との組み合わせのみで利用しているので、他の IME との組み合わせについては殆ど動作確認していません.
skk との組み合わせでは以下のような不具合がわかっています.

* @esc から未確定文字をコミット(確定)すると、`▼` 等もコミットされてしまう.

## License

Copyright (c) 2018 hankei6km

Licensed under the MIT License. See LICENSE.txt in the project root.