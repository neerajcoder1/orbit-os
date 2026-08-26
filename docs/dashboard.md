# Orbit OS Developer Dashboard

The Orbit OS Developer Dashboard provides a modern web interface to build, test, and monitor the operating system directly from your browser.

## Architecture

- **Backend (`dashboard/backend`)**: Built with FastAPI and SQLAlchemy (PostgreSQL). Exposes endpoints to trigger OS compilation (`make`) and automated QEMU integration tests.
- **Frontend (`dashboard/frontend`)**: Built with React, TypeScript, and Vite. Provides a unified view of test history and a live terminal output pane.

## Getting Started

### 1. Backend Setup
1. Ensure Python 3.10+ and PostgreSQL are installed.
2. Navigate to `dashboard/backend`.
3. Create a virtual environment: `python3 -m venv venv`
4. Install dependencies: `pip install -r requirements.txt`
5. Set `DATABASE_URL` (optional, defaults to `postgresql://postgres:postgres@localhost:5432/orbitos`).
6. Run the server: `uvicorn main:app --reload` (Runs on port 8000).

### 2. Frontend Setup
1. Ensure Node.js and NPM are installed.
2. Navigate to `dashboard/frontend`.
3. Install dependencies: `npm install`
4. Start the development server: `npm run dev` (Runs on port 5173).

## Features
- **One-Click Build**: Click "Build OS" to compile the C and Assembly codebase. The compiler's stdout and stderr stream directly into the dashboard.
- **Automated QEMU Testing**: Click a test button to trigger a QEMU instance in the background. The backend automates keyboard strokes into QEMU, asserts against the serial output, and saves the pass/fail result to the PostgreSQL database.
- **Test History**: A real-time timeline of test results is displayed alongside the logs.
