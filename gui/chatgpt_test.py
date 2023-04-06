import sys
from PyQt5.QtWidgets import QApplication
from PyQt5.QtWebEngineWidgets import QWebEngineView
from PyQt5.QtCore import Qt, QUrl

if __name__ == '__main__':
    #QApplication.setAttribute(Qt.AA_UseSoftwareOpenGL)
    # Create a PyQt5 application instance
    app = QApplication(sys.argv)


    # Create a QWebEngineView instance
    view = QWebEngineView()

    # Load the HTML content into the view
    view.setHtml("<html><body><h1>OpenStreetMap</h1></body></html>")
    #view.load(QUrl("https://www.google.com/"))

    view.setFixedSize(640, 480)

    # Show the view
    view.show()

    # Start the application event loop
    sys.exit(app.exec_())
