//
//  ContentView.swift
//  TestScrollEdge
//
//  Created by Sun Junwen on 2026/8/19.
//

import SwiftUI

struct ContentView: View {
    static let windowWidth: CGFloat = 640
    static let windowHeight: CGFloat = 360

    private static let sampleText: String = """
    文件名: /Users/ipsum/Downloads/draw.io-universal-28.1.2.dmg
    文件大小: 236846682 字节 (236.85 MB)
    修改日期: 2025-09-24 10:01
    MD5: FBF44C3C30B6D3D91E492D2E6D8F3B35
    SHA1: 6EBA6EB9106857E5F08CA7BB4E4F4F8D21DDA74E
    SHA256: 39E903695AEC303F3160DC9F43094F70765D138DFCD9850F101D5E5127EE8DF9
    SHA512: 6A92152C740E9742EB8EBDD150848D98AE472149832F978B573130C591C5B93C15398FFFA2C739C71CFFECE07BAED063D5D7EF2B86E7A2B42E1DA68D2B1E125C

    文件名: /Users/ipsum/Downloads/ffmpeg-109856-gf8d6d0fbf1.7z
    文件大小: 16660254 字节 (16.66 MB)
    修改日期: 2023-02-15 11:33
    MD5: 4C1A1D7D8380C41091428E5E8C06EFE2
    SHA1: 151FB04238FE07AD1DDD07EDB239CCC393BC19A0
    SHA256: 192877F68D8417649BBFFD12756DFB4D9991ED4C90854E6861CC70D4D4CE2DC0
    SHA512: 9140C95222BBD494F6089C45D5C3AAF17B4F350A90241BFC2927202E116834820D241CCE84E1FAECFE75E7455CEADA066FDC8FE024350BE36AA7F9D20595A226

    文件名: /Users/ipsum/Downloads/fHash-26.8.48-macOS.dmg
    文件大小: 2872505 字节 (2.87 MB)
    修改日期: 2026-08-03 13:21
    MD5: AFB34C1B69487CEDB9BA375C8FFA7F9B
    SHA1: 4DD191619935D8377AEAFD743794642D508253CF
    SHA256: 26A254E34CBF8D7708E839ABF8B1307C5E596C26BA7A37ACE3944637D19AE1A7
    SHA512: 69D27F33ADBD2B3AE19D3C1EC5E903AD028E55B8B46C86EB4CC01DD21074F5943D3079C430835F98C50016932933002BC45D9903F5CE93B5985DEF6ADD2EE514

    文件名: /Users/ipsum/Downloads/FileZilla_3.70.6_macos-arm64.app.tar.bz2
    文件大小: 13914406 字节 (13.91 MB)
    修改日期: 2026-06-16 14:12
    MD5: E45A7B121BD4442E8E410D162F12E401
    SHA1: 1CAAEF6D465E2973190DD516F3ED56CC99FE186A
    SHA256: F2A2C13361E1037BAB9729A4D738F6CE6B489F04FD82572CA1B0235B602BDFCA
    SHA512: A15505CEA4856E12A15B13880021FE14B0A432719C867E5FF8C0F826FF5C811797AF760D9311BA988ED563BF7D86CF04D4DF87FB6F42E604B11E658ED21A5650

    文件名: /Users/ipsum/Downloads/Notepad++-8.6.9-x64-installer.exe
    文件大小: 5823641 字节 (5.82 MB)
    修改日期: 2025-11-30 21:08
    MD5: 7E3C9F1A8D2B5E4C6A9F3B1D8E5C2A4F
    SHA1: 4B8C2F6A1D9E3B5C7A2F4D6B8E1C3A5F7B9D2E4C
    SHA256: 2C5F8B1A4D7E3C6F9B2A5D8C1E4F7B3A6D9C2E5F8B1A4D7E3C6F9B2A5D8C1E4F
    SHA512: 8D3A6E9C2F5B1D4A7E8C3F6B9D2E5A1C4F7B8D3A6E9C2F5B1D4A7E8C3F6B9D2E5A1C4F7B8D3A6E9C2F5B1D4A7E8C3F6B9D2E5A1C4F7B8D3A6E9C

    文件名: /Users/ipsum/Downloads/ubuntu-26.04-desktop-amd64.iso
    文件大小: 6123685892 字节 (5.70 GB)
    修改日期: 2026-04-25 18:14
    MD5: 1B4D7E2A5C8F3B6D9E1A4C7F2B5D8E3A
    SHA1: 6F2B5D8E1A4C7F3B6D9E2A5C8F1B4D7E3A6F2B5D
    SHA256: 6F2B5D8E1A4C7F3B6D9E2A5C8F1B4D7E3A6F2B5D8E1A4C7F3B6D9E2A5C8F1B4D
    SHA512: 9A4C7F2B5D8E1A6F3B9D2E5C8F1A4D7E2B6F9A3C5D8E1B4F7A2D5C8E3B6F9A1D4C7E2B5F8A3D6C9E1B4F7A2D5C8E3B6F9A1D4C7E2B5F8A3D6C9E1B4F7
    """

    var body: some View {
        GeometryReader { proxy in
            ScrollView([.horizontal, .vertical], showsIndicators: true) {
                Text(Self.sampleText)
                    .font(.system(size: 12, design: .monospaced))
                    .textSelection(.enabled)
                    .lineLimit(nil)
                    .fixedSize(horizontal: true, vertical: true)
                    .padding(.horizontal, 18)
                    .padding(.top, 4)
                    .padding(.bottom, 30)
                    .frame(
                        minWidth: proxy.size.width,
                        minHeight: proxy.size.height,
                        alignment: .topLeading
                    )
            }
            .scrollEdgeEffectStyle(.soft, for: .vertical)
            //.scrollEdgeEffectStyle(.hard, for: .vertical)
        }
        .frame(
            minWidth: Self.windowWidth,
            idealWidth: Self.windowWidth,
            maxWidth: .infinity,
            minHeight: Self.windowHeight,
            idealHeight: Self.windowHeight,
            maxHeight: .infinity,
            alignment: .topLeading
        )
        .toolbar {
            // Left flexible space — pushes the right item to the trailing edge
            ToolbarSpacer(.flexible)

            // Right
            ToolbarItem(placement: .primaryAction) {
                Button {
                    // Search action placeholder
                } label: {
                    Label("Search", systemImage: "magnifyingglass")
                }
                .help("Search")
            }
        }
    }
}

#Preview {
    ContentView()
}
