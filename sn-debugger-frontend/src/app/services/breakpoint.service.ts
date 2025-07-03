import { Injectable } from '@angular/core';

@Injectable({
  providedIn: 'root'
})
export class BreakpointService {
  // Track which breakpoints are toggled
  breakpointCol = new Set<string>();
  breakpointGroupCol = new Map<string, number>();
  currentBreakpoint: [string, number] | undefined = undefined;
  currentFrames: any[] | undefined = undefined;

  constructor() {}

  setbreakpoint(breakpoint: [string, number]): void {
    const [code, index] = breakpoint;
    const breakpointId = `${code}_${index}`;

    let count = this.breakpointGroupCol.get(code) ?? 0;

    if (this.breakpointCol.has(breakpointId)) {
      this.breakpointCol.delete(breakpointId);
      count--;
      if (count <= 0) {
        this.breakpointGroupCol.delete(code);
      } else {
        this.breakpointGroupCol.set(code, count);
      }
    } else {
      this.breakpointCol.add(breakpointId);
      this.breakpointGroupCol.set(code, count + 1);
    }
  }

  getGroupBreakpoint(code: string): boolean {
    return (this.breakpointGroupCol.get(code) ?? 0) > 0;
  }

  breakpointclass(breakpoint: [string, number]): string {
    const breakpointId = `${breakpoint[0]}_${breakpoint[1]}`;

    if (!breakpoint || !this.currentBreakpoint) {
      return 'error';
    }

    if (breakpoint[0] === this.currentBreakpoint[0] && breakpoint[1] === this.currentBreakpoint[1]) {
      return this.breakpointCol.has(breakpointId) ? 'breakpointsetat' : 'breakpointat';
    }

    let eclass = 'breakpointclear';

    if (this.currentFrames) {
      for (const frame of this.currentFrames) {
        if (breakpoint[0] === frame.breakpoint[0] && breakpoint[1] === frame.breakpoint[1]) {
          eclass = this.breakpointCol.has(breakpointId) ? 'breakpointsetatstack' : 'breakpointatstack';
          break;
        }
      }
    }

    if (eclass === 'breakpointclear') {
      if (this.breakpointCol.has(breakpointId)) {
        eclass = 'breakpointset';
      } else if (this.getGroupBreakpoint(breakpoint[0])) {
        eclass = 'breakpointgroupset';
      }
    }

    return eclass;
  }

  breakpointdefaultclass(breakpoint: [string, number], fallback: string): string {
    return `${this.breakpointclass(breakpoint)} ${fallback}`;
  }

  updateState(current: [string, number], frames: any[]): void {
    this.currentBreakpoint = current;
    this.currentFrames = frames;
  }
}
