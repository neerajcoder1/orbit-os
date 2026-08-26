from fastapi import FastAPI, Depends, BackgroundTasks
from fastapi.middleware.cors import CORSMiddleware
from sqlalchemy.orm import Session
import subprocess
import os

from database import engine, get_db
import models

models.Base.metadata.create_all(bind=engine)

app = FastAPI(title="Orbit OS Developer Dashboard API")

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_methods=["*"],
    allow_headers=["*"],
)

OS_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), "../../"))

@app.get("/")
def read_root():
    return {"status": "Orbit OS Dashboard API is running"}

@app.post("/build")
def trigger_build():
    try:
        # Run make all iso in the OS directory
        result = subprocess.run(
            ["make", "clean", "all", "iso"],
            cwd=OS_DIR,
            capture_output=True,
            text=True
        )
        return {
            "success": result.returncode == 0,
            "stdout": result.stdout,
            "stderr": result.stderr
        }
    except Exception as e:
        return {"success": False, "error": str(e)}

@app.post("/test/{test_name}")
def run_test(test_name: str, db: Session = Depends(get_db)):
    if test_name not in ["test_vfs", "test_drivers"]:
        return {"success": False, "error": "Unknown test"}
        
    try:
        # We assume WSL is required to run the QEMU tests on Windows properly
        result = subprocess.run(
            ["wsl", "-u", "root", "python3", f"{test_name}.py"],
            cwd=OS_DIR,
            capture_output=True,
            text=True
        )
        
        success = result.returncode == 0
        log_output = result.stdout + "\n" + result.stderr
        
        # Save to database
        db_run = models.TestRun(
            test_name=test_name,
            success=success,
            log_output=log_output
        )
        db.add(db_run)
        db.commit()
        db.refresh(db_run)
        
        return {
            "success": success,
            "stdout": result.stdout,
            "stderr": result.stderr,
            "run_id": db_run.id
        }
    except Exception as e:
        return {"success": False, "error": str(e)}

@app.get("/tests")
def get_test_history(db: Session = Depends(get_db)):
    runs = db.query(models.TestRun).order_by(models.TestRun.timestamp.desc()).limit(20).all()
    return runs
