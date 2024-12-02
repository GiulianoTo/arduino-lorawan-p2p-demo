import sys
import serial
import serial.tools.list_ports
from PyQt6.QtWidgets import QApplication, QPushButton, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout, QTextEdit, QLabel, QComboBox, QLineEdit, QMessageBox
from PyQt6.QtGui import QPixmap, QTextCursor  
from PyQt6.QtCore import QThread, pyqtSignal, Qt

class SerialReaderThread(QThread):
    data_received = pyqtSignal(str)

    def __init__(self, serial_port):
        super().__init__()
        self.serial_port = serial_port
        self.running = True

    def run(self):
        while self.running:
            if self.serial_port.in_waiting > 0:
                data = self.serial_port.read(self.serial_port.in_waiting).decode('utf-8', errors='ignore')
                self.data_received.emit(data)

    def stop(self):
        self.running = False
        self.serial_port.close()

class SerialPanel(QMainWindow):
    def __init__(self):
        super().__init__()

        self.setWindowTitle("Serial Communication Panel")
        self.setGeometry(100, 100, 600, 400)

        # Set background color to black
        self.setStyleSheet("background-color: black; color: white;")

        # Serial port setup
        self.serial_port = None
        self.serial_thread = None

        # Main layout
        main_layout = QVBoxLayout()

        # Add logo at the top
        logo_label = QLabel()
        pixmap = QPixmap("./logo.png")
        logo_label.setPixmap(pixmap)
        logo_label.setAlignment(Qt.AlignmentFlag.AlignCenter)
        main_layout.addWidget(logo_label)

        # Serial output display
        self.output_display = QTextEdit()
        self.output_display.setReadOnly(True)
        self.output_display.setStyleSheet("""
            background-color: #333;
            color: white;
            font-size: 16px;  /* Aumenta la dimensione del font */
        """)
        main_layout.addWidget(QLabel("Received Data:"))
        main_layout.addWidget(self.output_display)

        # Serial connection controls
        port_layout = QHBoxLayout()
        self.port_select = QComboBox()
        self.port_select.setStyleSheet("background-color: #333; color: white;")
        self.refresh_ports()
        port_layout.addWidget(QLabel("Serial Port:"))
        port_layout.addWidget(self.port_select)
        
        self.baudrate_input = QLineEdit("9600")
        self.baudrate_input.setStyleSheet("background-color: #333; color: white;")
        port_layout.addWidget(QLabel("Baudrate:"))
        port_layout.addWidget(self.baudrate_input)

        self.connect_button = QPushButton("Connect")
        self.connect_button.setStyleSheet("""
            QPushButton {
                background-color: #6200EE;
                color: white;
                border-radius: 8px;
                padding: 10px;
            }
            QPushButton:hover {
                background-color: #3700B3;
            }
            QPushButton:pressed {
                background-color: #03DAC5;
            }
        """)
        self.connect_button.clicked.connect(self.toggle_connection)
        port_layout.addWidget(self.connect_button)
        
        main_layout.addLayout(port_layout)

        # Set main widget and layout
        container = QWidget()
        container.setLayout(main_layout)
        self.setCentralWidget(container)

    def refresh_ports(self):
        """Refresh the list of available serial ports."""
        self.port_select.clear()
        ports = serial.tools.list_ports.comports()
        self.port_select.addItems([port.device for port in ports])

    def toggle_connection(self):
        """Connect or disconnect from the selected serial port."""
        port = self.port_select.currentText()
        
        # Verifica se la porta esiste nel sistema
        available_ports = [p.device for p in serial.tools.list_ports.comports()]
        if port not in available_ports:
            QMessageBox.critical(self, "Error", f"The selected port '{port}' does not exist.")
            return

        # Connessione o disconnessione
        if self.serial_port and self.serial_port.is_open:
            # Disconnect
            self.serial_thread.stop()
            self.serial_thread = None
            self.serial_port = None
            self.connect_button.setText("Connect")
        else:
            # Connect
            baudrate = int(self.baudrate_input.text())
            self.serial_port = serial.Serial(port, baudrate, timeout=1)
            self.serial_thread = SerialReaderThread(self.serial_port)
            self.serial_thread.data_received.connect(self.display_received_data)
            self.serial_thread.start()
            self.connect_button.setText("Disconnect")

    def display_received_data(self, data):
        """Display received data in the text edit."""
        # Handle newline and carriage return characters
        # formatted_data = data.replace("\r\n", "\n").replace("\r", "\n")
        formatted_data = data
        self.output_display.insertPlainText(formatted_data)
        self.output_display.moveCursor(QTextCursor.MoveOperation.End)  # Scorri fino alla fine

    def closeEvent(self, event):
        """Handle closing of the application and stop the serial thread."""
        if self.serial_thread:
            self.serial_thread.stop()
        event.accept()

app = QApplication(sys.argv)
window = SerialPanel()
window.show()
sys.exit(app.exec())
