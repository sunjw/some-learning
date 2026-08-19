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

@main
struct TestScrollEdgeApp: App {
    @NSApplicationDelegateAdaptor(AppDelegate.self) private var appDelegate

    var body: some Scene {
        WindowGroup {
            ContentView()
        }
        .windowStyle(.titleBar)
        .windowToolbarStyle(.unified(showsTitle: false))
        .defaultSize(width: ContentView.windowWidth, height: ContentView.windowHeight)
        .windowResizability(.contentMinSize)
    }
}
