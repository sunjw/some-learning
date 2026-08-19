//
//  TestScrollEdgeApp.swift
//  TestScrollEdge
//
//  Created by Sun Junwen on 2026/8/19.
//

import AppKit
import SwiftUI

final class AppDelegate: NSObject, NSApplicationDelegate {
    func applicationShouldTerminateAfterLastWindowClosed(_ sender: NSApplication) -> Bool {
        true
    }
}

extension View {
    @ViewBuilder
    func scrollEdgeSoftIfAvailable() -> some View {
        // *OS 26 defaults to `.soft`, while *OS 27 defaults to `.hard`.
        // Reset to .soft
        self.scrollEdgeEffectStyle(.soft, for: .all)
    }
}

@main
struct TestScrollEdgeApp: App {
    @NSApplicationDelegateAdaptor(AppDelegate.self) private var appDelegate

    var body: some Scene {
        WindowGroup {
            ContentView()
                .scrollEdgeSoftIfAvailable()
        }
        .windowStyle(.titleBar)
        .windowToolbarStyle(.unified(showsTitle: false))
        .windowResizability(.contentMinSize)
        .defaultSize(width: ContentView.windowWidth, height: ContentView.windowHeight)
        .defaultPosition(.center)
    }
}
