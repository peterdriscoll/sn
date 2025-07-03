import { Injectable } from '@angular/core';

@Injectable({
  providedIn: 'root'
})
export class RerunCommandService {

  rerunToStackPoint(stepcount: number): void {
    const stepCountField = document.getElementById('stepcountid') as HTMLInputElement;
    if (stepCountField) {
      stepCountField.value = stepcount.toString();
      stepCountField.focus();
      stepCountField.scrollIntoView({ behavior: 'smooth', block: 'end', inline: 'nearest' });
    }
  }

  updateRerunCommand(stepcount: number): void {
    const rerunField = document.getElementById('reruncommandid') as HTMLInputElement;
    if (rerunField) {
      rerunField.value = `rerun ${stepcount}`;
    }
  }

  executeRerun(stepcount: number): void {
    this.updateRerunCommand(stepcount);
    this.rerunToStackPoint(stepcount);
  }
} 
