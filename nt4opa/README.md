# nt4opa

CH32V003内蔵オペアンプを使う。（CH32V003funのadc_dma_opampを参考にしている）

オペアンプの非反転増幅回路を構成して、可変抵抗で作成した分圧をオペアンプ（OPP0）、表示用（PD3/A4）に入力する。また、オペアンプの出力OPOもAD変換してコンソール出力している。

２つのAD変換の値（A4、A7）はDMAでグローバル変数のバッファadc_bufferに自動的にシーケンス順に交互にコピーされるので、下表のようにadc_bufferバッファを参照するだけで各AD値が取得できる。というサンプル。

コンソール出力にはRDATAを直接参照して同時に出力している。この値は、SQ1とSQ2のどちらかの値と一致する。

|シーケンス番号|AD変換|バッファ|
|--|--|--|
|SQ1| PD3/A4 | adc_buffer[0] |
|SQ2| PD4/A7(OPO) | adc_buffer[1] |


## ビルド＆実行方法

```bash
> make clean && make && ../ch32v003fun/minichlink/minichlink -T
:
[VR(SQ1)]   2, [OPO(SQ2)]   8, [ADC1->RDATA]   2
[VR(SQ1)]   2, [OPO(SQ2)]  11, [ADC1->RDATA]   2
[VR(SQ1)]   2, [OPO(SQ2)]  12, [ADC1->RDATA]  12
[VR(SQ1)]  57, [OPO(SQ2)] 297, [ADC1->RDATA] 297
[VR(SQ1)]  92, [OPO(SQ2)] 575, [ADC1->RDATA] 575
[VR(SQ1)] 105, [OPO(SQ2)] 615, [ADC1->RDATA] 105
[VR(SQ1)] 105, [OPO(SQ2)] 646, [ADC1->RDATA] 105
 :
```

## 回路図

<img src="circuit.png" width=500>
