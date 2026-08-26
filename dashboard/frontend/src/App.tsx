import { useState, useEffect } from 'react';
import './App.css';

function App() {
  const [loading, setLoading] = useState(false);
  const [logs, setLogs] = useState("");
  const [history, setHistory] = useState([]);

  const API_URL = "http://localhost:8000";

  const fetchHistory = async () => {
    try {
      const res = await fetch(`${API_URL}/tests`);
      const data = await res.json();
      setHistory(data);
    } catch (e) {
      console.error(e);
    }
  };

  useEffect(() => {
    fetchHistory();
  }, []);

  const runBuild = async () => {
    setLoading(true);
    setLogs("Building Orbit OS...\n");
    try {
      const res = await fetch(`${API_URL}/build`, { method: "POST" });
      const data = await res.json();
      setLogs(prev => prev + data.stdout + "\n" + data.stderr);
    } catch (e) {
      setLogs(prev => prev + "\nError: " + String(e));
    }
    setLoading(false);
  };

  const runTest = async (testName: string) => {
    setLoading(true);
    setLogs(`Running ${testName}...\n`);
    try {
      const res = await fetch(`${API_URL}/test/${testName}`, { method: "POST" });
      const data = await res.json();
      setLogs(prev => prev + data.stdout + "\n" + data.stderr);
      fetchHistory();
    } catch (e) {
      setLogs(prev => prev + "\nError: " + String(e));
    }
    setLoading(false);
  };

  return (
    <div style={{ padding: "2rem", fontFamily: "sans-serif" }}>
      <h1>Orbit OS Developer Dashboard</h1>
      
      <div style={{ display: "flex", gap: "1rem", marginBottom: "2rem" }}>
        <button onClick={runBuild} disabled={loading} style={{ padding: "0.5rem 1rem" }}>
          Build OS (make all iso)
        </button>
        <button onClick={() => runTest("test_vfs")} disabled={loading} style={{ padding: "0.5rem 1rem" }}>
          Run VFS Test
        </button>
        <button onClick={() => runTest("test_drivers")} disabled={loading} style={{ padding: "0.5rem 1rem" }}>
          Run Drivers/Network Test
        </button>
      </div>

      <div style={{ display: "flex", gap: "2rem" }}>
        <div style={{ flex: 2 }}>
          <h2>Live Terminal Output</h2>
          <pre style={{ 
            backgroundColor: "#1e1e1e", 
            color: "#00ff00", 
            padding: "1rem", 
            height: "500px", 
            overflowY: "auto",
            borderRadius: "4px"
          }}>
            {logs || "Waiting for command..."}
          </pre>
        </div>

        <div style={{ flex: 1 }}>
          <h2>Test History</h2>
          <ul style={{ listStyle: "none", padding: 0 }}>
            {history.map((run: any) => (
              <li key={run.id} style={{ 
                padding: "1rem", 
                marginBottom: "0.5rem", 
                backgroundColor: run.success ? "#d4edda" : "#f8d7da",
                color: run.success ? "#155724" : "#721c24",
                borderRadius: "4px"
              }}>
                <strong>{run.test_name}</strong> - {run.success ? "PASS" : "FAIL"}
                <br />
                <small>{new Date(run.timestamp).toLocaleString()}</small>
              </li>
            ))}
          </ul>
        </div>
      </div>
    </div>
  );
}

export default App;
