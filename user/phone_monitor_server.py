#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import http.server
import socketserver
import json
import subprocess
import sys
import os
import time
from urllib.parse import urlparse, parse_qs

# Configuration
PORT = 8080
KPTOOLS_PATH = "./kptools"  # Path to kptools
KPM_PATH = "./phone-monitor.kpm"  # Path to KPM file

# Global variables
module_loaded = False

class PhoneMonitorHandler(http.server.BaseHTTPRequestHandler):
    def do_GET(self):
        global module_loaded
        
        # Parse URL
        parsed_url = urlparse(self.path)
        path = parsed_url.path
        query_params = parse_qs(parsed_url.query)
        
        # Serve main page
        if path == "/":
            self.serve_main_page()
        # Serve phone information
        elif path == "/info":
            self.serve_phone_info()
        # Load module
        elif path == "/load":
            self.load_module()
        # Unload module
        elif path == "/unload":
            self.unload_module()
        # Serve static files (CSS, JS, etc.)
        elif path.startswith("/static/"):
            self.serve_static_file(path)
        else:
            self.send_error(404, "Page not found")
    
    def serve_main_page(self):
        """Serve the main HTML page"""
        html_content = """
<!DOCTYPE html>
<html>
<head>
    <title>Phone Monitor</title>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: Arial, sans-serif; margin: 40px; background-color: #f0f0f0; }
        .container { max-width: 800px; margin: 0 auto; background-color: white; padding: 20px; border-radius: 10px; box-shadow: 0 0 10px rgba(0,0,0,0.1); }
        h1 { color: #333; text-align: center; }
        .controls { text-align: center; margin: 20px 0; }
        button { background-color: #4CAF50; border: none; color: white; padding: 10px 20px; text-align: center; text-decoration: none; display: inline-block; font-size: 16px; margin: 4px 2px; cursor: pointer; border-radius: 5px; }
        button:hover { background-color: #45a049; }
        #info { background-color: #f9f9f9; border: 1px solid #ddd; padding: 15px; margin: 20px 0; border-radius: 5px; min-height: 200px; }
        .status { text-align: center; padding: 10px; margin: 10px 0; border-radius: 5px; }
        .loaded { background-color: #dff0d8; color: #3c763d; }
        .unloaded { background-color: #f2dede; color: #a94442; }
    </style>
</head>
<body>
    <div class="container">
        <h1>Phone Monitor</h1>
        
        <div class="controls">
            <button onclick="loadModule()">Load Module</button>
            <button onclick="unloadModule()">Unload Module</button>
            <button onclick="refreshInfo()">Refresh Info</button>
        </div>
        
        <div id="status" class="status unloaded">Module is not loaded</div>
        
        <div id="info">
            <p>Click "Load Module" to start monitoring your phone.</p>
        </div>
    </div>

    <script>
        // Check module status on page load
        window.onload = function() {
            refreshInfo();
        };
        
        function loadModule() {
            fetch('/load')
                .then(response => response.text())
                .then(data => {
                    alert(data);
                    refreshInfo();
                })
                .catch(error => {
                    alert('Error loading module: ' + error);
                });
        }
        
        function unloadModule() {
            fetch('/unload')
                .then(response => response.text())
                .then(data => {
                    alert(data);
                    refreshInfo();
                })
                .catch(error => {
                    alert('Error unloading module: ' + error);
                });
        }
        
        function refreshInfo() {
            fetch('/info')
                .then(response => response.text())
                .then(html => {
                    document.getElementById('info').innerHTML = html;
                    
                    // Update status
                    const statusElement = document.getElementById('status');
                    if (html.includes('Phone Information')) {
                        statusElement.className = 'status loaded';
                        statusElement.textContent = 'Module is loaded';
                    } else {
                        statusElement.className = 'status unloaded';
                        statusElement.textContent = 'Module is not loaded';
                    }
                })
                .catch(error => {
                    document.getElementById('info').innerHTML = '<p>Error fetching information: ' + error + '</p>';
                });
        }
    </script>
</body>
</html>
        """
        
        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.end_headers()
        self.wfile.write(html_content.encode('utf-8'))
    
    def serve_phone_info(self):
        """Serve phone information from the kernel module"""
        global module_loaded
        
        if not module_loaded:
            self.send_response(200)
            self.send_header('Content-type', 'text/html')
            self.end_headers()
            self.wfile.write(b"<p>Module is not loaded. Click 'Load Module' to start monitoring.</p>")
            return
        
        try:
            # Call kptools to get information from the kernel module
            # This is a simplified example - in practice, you would need to implement
            # the proper communication with the kernel module
            result = subprocess.run([KPTOOLS_PATH, "control", KPM_PATH, "get_info"], 
                                  capture_output=True, text=True, timeout=10)
            
            if result.returncode == 0:
                # If kptools returns HTML, serve it directly
                if result.stdout.strip().startswith("<"):
                    html_content = result.stdout
                else:
                    # If it returns plain text, wrap it in HTML
                    html_content = f"<p>{result.stdout.replace(chr(10), '<br>')}</p>"
            else:
                html_content = f"<p>Error getting information: {result.stderr}</p>"
        except subprocess.TimeoutExpired:
            html_content = "<p>Error: Timeout while getting information</p>"
        except Exception as e:
            html_content = f"<p>Error getting information: {str(e)}</p>"
        
        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.end_headers()
        self.wfile.write(html_content.encode('utf-8'))
    
    def load_module(self):
        """Load the kernel module"""
        global module_loaded
        
        try:
            # Check if kptools exists
            if not os.path.exists(KPTOOLS_PATH):
                self.send_response(200)
                self.send_header('Content-type', 'text/plain')
                self.end_headers()
                self.wfile.write(b"Error: kptools not found")
                return
            
            # Check if KPM exists
            if not os.path.exists(KPM_PATH):
                self.send_response(200)
                self.send_header('Content-type', 'text/plain')
                self.end_headers()
                self.wfile.write(b"Error: KPM file not found")
                return
            
            # Load the module using kptools
            result = subprocess.run([KPTOOLS_PATH, "load", KPM_PATH], 
                                  capture_output=True, text=True, timeout=30)
            
            if result.returncode == 0:
                module_loaded = True
                response_text = "Module loaded successfully"
            else:
                response_text = f"Error loading module: {result.stderr}"
        except subprocess.TimeoutExpired:
            response_text = "Error: Timeout while loading module"
        except Exception as e:
            response_text = f"Error loading module: {str(e)}"
        
        self.send_response(200)
        self.send_header('Content-type', 'text/plain')
        self.end_headers()
        self.wfile.write(response_text.encode('utf-8'))
    
    def unload_module(self):
        """Unload the kernel module"""
        global module_loaded
        
        try:
            # Unload the module using kptools
            result = subprocess.run([KPTOOLS_PATH, "unload", KPM_PATH], 
                                  capture_output=True, text=True, timeout=30)
            
            if result.returncode == 0:
                module_loaded = False
                response_text = "Module unloaded successfully"
            else:
                response_text = f"Error unloading module: {result.stderr}"
        except subprocess.TimeoutExpired:
            response_text = "Error: Timeout while unloading module"
        except Exception as e:
            response_text = f"Error unloading module: {str(e)}"
        
        self.send_response(200)
        self.send_header('Content-type', 'text/plain')
        self.end_headers()
        self.wfile.write(response_text.encode('utf-8'))
    
    def serve_static_file(self, path):
        """Serve static files (CSS, JS, images, etc.)"""
        # For simplicity, we'll just return a 404 for static files in this example
        self.send_error(404, "Static files not implemented in this example")

def main():
    """Main function to start the HTTP server"""
    try:
        with socketserver.TCPServer(("", PORT), PhoneMonitorHandler) as httpd:
            print(f"Phone Monitor Server started at http://localhost:{PORT}")
            print("Press Ctrl+C to stop the server")
            httpd.serve_forever()
    except KeyboardInterrupt:
        print("\nServer stopped")
    except Exception as e:
        print(f"Error starting server: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()